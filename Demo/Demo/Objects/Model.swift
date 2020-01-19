//
//  Model.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import SceneKit

/// A data structure to wrap around `cmdl_t` and provide SceneKit data types.
struct Model {

    // MARK: - Private Properties

    /// The backing CMDL of this model.
    private let backing: cmdl_t

    // MARK: - Initializers

    /// - Parameter backing: The backing CMDL of this model.
    init(backing: cmdl_t) {
        self.backing = backing
    }

    // MARK: - Public Methods

    /// Get the `SCNNode` representation of this model's backing, if any.
    /// - Parameter file: The file to read this model's backing data from.
    /// - Returns: The `SCNNode` representation of this model's backing, if any.
    mutating func getNode(from file: UnsafeMutablePointer<FILE>) -> SCNNode {
        let node = SCNNode()

        // create the nodes for each object
        for index in 0..<Int(backing.num_objects) {
            guard let object = backing.object_sobjs[index]?.pointee.object?.pointee else {
                continue
            }

            guard let materialMtob = backing.materials[Int(object.material_index)]?.pointee else {
                continue
            }

            let material = Material(backing: materialMtob).getMaterial()
            let objectNode = getNode(for: object, in: backing, material: material, file: file)
            node.addChildNode(objectNode)
        }

        // apply the model attributes
        let scale = SCNVector3(backing.transform_scale)
        let rotation = SCNVector4(backing.transform_rotation)
        let translation = SCNVector3(backing.transform_translation)
        let local = SCNMatrix4(backing.transform_local)
        let world = SCNMatrix4(backing.transform_world)

        node.scale = scale
        node.rotation = rotation
        node.position = translation
        node.transform = local
        node.isHidden = !backing.is_visible
        node.setWorldTransform(world)
        return node
    }

    // MARK: - Private Methods

    /// Get the SceneKit node representation of the given object from the given CMDL in the given file with the given material.
    /// - Parameters:
    ///   - object: The object to get the representation of.
    ///   - model: The CMDL that contains the given object.
    ///   - material: The material to use for the given object's geometry.
    ///   - file: The pointer to the file handle to read the mesh vertex data from.
    /// - Returns: The SceneKit node representation of the given object.
    private func getNode(for object: sobj_object_t, in model: cmdl_t, material: SCNMaterial, file: UnsafeMutablePointer<FILE>) -> SCNNode {
        guard let mesh = model.mesh_sobjs[Int(object.mesh_index)]?.pointee.mesh?.pointee else {
            fatalError()
        }

        // create nodes for each of the meshes faces and vertex groups
        // create the geometry elements for each face group
        var elements = [SCNGeometryElement]()
        for index in 0..<Int(mesh.num_face_groups) {
            guard let group = mesh.face_groups[index]?.pointee, let indices = group.indices else {
                continue
            }

            // double the count to get the size in bytes, each item is u16
            let data = Data(bytes: indices, count: Int(group.num_indices) * 2)
            let element = SCNGeometryElement(
                data: data,
                primitiveType: .triangles,
                primitiveCount: Int(group.num_indices) / 3,
                bytesPerIndex: 2
            )

            elements.append(element)
        }

        // create the geometry sources for each vertex group
        var sources = [SCNGeometrySource]()
        for index in 0..<Int(mesh.num_vertex_groups) {
            guard var group = mesh.vertex_groups[index]?.pointee else {
                continue
            }

            guard let decoded = sobj_vertex_group_decode(&group, file) else {
                continue
            }

            let data = Data(bytes: decoded, count: Int(group.decoded_data_size))
            let groupSources = geometrySources(from: data, in: group)
            sources.append(contentsOf: groupSources)
            decoded.deallocate()
        }

        // create the geometry and node
        let geometry = SCNGeometry(sources: sources, elements: elements)
        geometry.firstMaterial = material

        let node = SCNNode(geometry: geometry)
        let translation = SCNVector3(mesh.transform_translation)
        node.position = translation

        // return the node
        return node
    }

    /// Create the geometry sources for the vertex components of the given vertex group, within the given vertex data.
    /// - Parameters:
    ///   - data: The vertex data to source the geometry from.
    ///   - vertexGroup: The vertex group for the given vertex data.
    private func geometrySources(from data: Data, in vertexGroup: sobj_vertex_group_t) -> [SCNGeometrySource] {
        var sources = [SCNGeometrySource]()
        var componentOffset = 0
        for componentIndex in 0..<Int(vertexGroup.num_components) {
            guard let component = vertexGroup.components[componentIndex]?.pointee else {
                continue
            }

            // *4 for float size
            let size = Int(component.num_values) * 4
            let semantic: SCNGeometrySource.Semantic
            switch component.type {
            case SOBJ_COMPONENT_TYPE_POSITION: semantic = .vertex
            case SOBJ_COMPONENT_TYPE_NORMAL:   semantic = .normal
            case SOBJ_COMPONENT_TYPE_COLOR:    semantic = .color
            case SOBJ_COMPONENT_TYPE_UV0:      semantic = .texcoord
            case SOBJ_COMPONENT_TYPE_UV1:      semantic = .texcoord
            case SOBJ_COMPONENT_TYPE_WEIGHT:   semantic = .boneWeights
            case SOBJ_COMPONENT_TYPE_INDEX:    componentOffset += size; continue
            default: continue
            }

            let source = SCNGeometrySource(
                data: data,
                semantic: semantic,
                vectorCount: Int(vertexGroup.num_vertices),
                usesFloatComponents: true,
                componentsPerVector: Int(component.num_values),
                bytesPerComponent: 4,
                dataOffset: Int(componentOffset),
                dataStride: Int(vertexGroup.decoded_data_stride)
            )

            sources.append(source)
            componentOffset += size
        }

        return sources
    }
}

// MARK: - SCNVector3 Extensions

extension SCNVector3 {
    init(_ vector: vec3_t) {
        self.init(
            x: CGFloat(vector.x),
            y: CGFloat(vector.y),
            z: CGFloat(vector.z)
        )
    }
}

// MARK: - SCNVector4 Extensions

extension SCNVector4 {
    init(_ vector: vec3_t) {
        self.init(
            x: CGFloat(vector.x),
            y: CGFloat(vector.y),
            z: CGFloat(vector.z),
            w: 0
        )
    }
}

// MARK: - SCNMatrix4 Extensions

extension SCNMatrix4 {
    init(_ matrix: mat4_t) {
        self.init(
            m11: CGFloat(matrix.data.0.0), m12: CGFloat(matrix.data.0.1), m13: CGFloat(matrix.data.0.2), m14: CGFloat(matrix.data.0.3),
            m21: CGFloat(matrix.data.1.0), m22: CGFloat(matrix.data.1.1), m23: CGFloat(matrix.data.1.2), m24: CGFloat(matrix.data.1.3),
            m31: CGFloat(matrix.data.2.0), m32: CGFloat(matrix.data.2.1), m33: CGFloat(matrix.data.2.2), m34: CGFloat(matrix.data.2.3),
            m41: CGFloat(matrix.data.3.0), m42: CGFloat(matrix.data.3.1), m43: CGFloat(matrix.data.3.2), m44: CGFloat(matrix.data.3.3)
        )
    }
}
