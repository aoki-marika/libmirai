//
//  CGFXMaterialsController.swift
//  Demo
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

import AppKit
import SceneKit

/// The controller for displaying materials within a CGFX.
class CGFXMaterialsController: NSViewController {

    // MARK: - Private Properties

    private let nameColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "NameColumn")
    private let activeTextureCoordinatorsColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "ActiveTextureCoordinatorsColumn")

    /// The scene of this controller used to display materials.
    private let scene = SCNScene()

    /// The sphere within the scene of this controller used to display materials.
    private var materialSphere: SCNSphere!

    /// The node within the scene of this controller that contains the material sphere.
    private var materialSphereNode: SCNNode!

    // MARK: - Public Properties

    /// The CGFX that this controller is currently displaying the materials of, if any.
    var cgfx: cgfx_t? {
        didSet {
            outlineView.reloadData()
        }
    }

    // MARK: - Outlets

    @IBOutlet private weak var outlineView: NSOutlineView!
    @IBOutlet weak var sceneView: SCNView!

    // MARK: - Public Methods

    override func viewDidLoad() {
        super.viewDidLoad()

        // setup the scene
        let camera = SCNCamera()
        camera.fieldOfView = 50
        camera.zFar = 1000

        let cameraNode = SCNNode()
        cameraNode.camera = camera
        cameraNode.position = .init(x: 15, y: 15, z: 15)
        cameraNode.look(at: .init(x: 0, y: 0, z: 0))
        scene.rootNode.addChildNode(cameraNode)

        let light = SCNLight()
        light.type = .directional

        let lightNode = SCNNode()
        lightNode.light = light
        lightNode.position = .init(x: 15, y: 15, z: 15)
        lightNode.look(at: .init(x: 0, y: 0, z: 0))
        scene.rootNode.addChildNode(lightNode)

        materialSphere = SCNSphere(radius: 8)
        materialSphereNode = SCNNode(geometry: materialSphere)
        materialSphereNode.isHidden = true
        scene.rootNode.addChildNode(materialSphereNode)

        // display the scene
        sceneView.scene = scene
    }
}

// MARK: - NSOutlineViewDataSource

extension CGFXMaterialsController: NSOutlineViewDataSource {

    func outlineView(_ outlineView: NSOutlineView, numberOfChildrenOfItem item: Any?) -> Int {
        guard let cgfx = cgfx else {
            return 0
        }

        guard let model = item as? cmdl_t else {
            return Int(cgfx.num_models)
        }

        return Int(model.num_materials)
    }

    func outlineView(_ outlineView: NSOutlineView, isItemExpandable item: Any) -> Bool {
        return item is cmdl_t
    }

    func outlineView(_ outlineView: NSOutlineView, child index: Int, ofItem item: Any?) -> Any {
        guard let cgfx = cgfx else {
            fatalError()
        }

        guard let model = item as? cmdl_t else {
            guard let model = cgfx.models[index]?.pointee else {
                fatalError()
            }

            return model
        }

        guard let material = model.materials[index]?.pointee else {
            fatalError()
        }

        return material
    }

    func outlineView(_ outlineView: NSOutlineView, objectValueFor tableColumn: NSTableColumn?, byItem item: Any?) -> Any? {
        switch tableColumn!.identifier {
        case nameColumnIdentifier:
            if let model = item as? cmdl_t, let cName = model.name {
                let name = String(cString: cName)
                return name
            }
            else if let material = item as? mtob_t, let cName = material.name {
                let name = String(cString: cName)
                return name
            }
            else {
                return nil
            }
        case activeTextureCoordinatorsColumnIdentifier:
            guard let material = item as? mtob_t else {
                return nil
            }

            return Int(material.num_active_texture_coordinators)
        default:
            return nil
        }
    }
}

// MARK: - NSOutlineViewDelegate

extension CGFXMaterialsController: NSOutlineViewDelegate {

    func outlineViewSelectionDidChange(_ notification: Notification) {
        guard let selectedMaterial = outlineView.item(atRow: outlineView.selectedRow) as? mtob_t else {
            materialSphereNode.isHidden = true
            return
        }

        let material = Material(backing: selectedMaterial)
        materialSphere.firstMaterial = material.getMaterial()
        materialSphereNode.isHidden = false
    }
}
