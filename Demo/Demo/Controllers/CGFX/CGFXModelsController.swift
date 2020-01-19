//
//  CGFXModelsController.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import AppKit
import SceneKit

/// The controller for displaying models within a CGFX.
class CGFXModelsController: NSViewController {

    // MARK: - Private Properties

    /// The scene of this controller used to display models.
    private let scene = SCNScene()

    /// The node of the currently displayed model within this controller, if any.
    private var modelNode: SCNNode?

    /// The currently selected model in this controller, if any.
    private var selectedModel: cmdl_t? {
        let selectedRow = tableView.selectedRow
        guard selectedRow >= 0 else {
            return nil
        }

        return cgfx?.models[selectedRow]?.pointee
    }

    // MARK: - Public Properties

    /// The CGFX that this controller is currently displaying the models of, if any.
    var cgfx: cgfx_t? {
        didSet {
            tableView.reloadData()
        }
    }

    // MARK: - Outlets

    @IBOutlet private weak var tableView: NSTableView!
    @IBOutlet private weak var sceneView: SCNView!

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

        // display the scene
        sceneView.scene = scene
    }
}

// MARK: - NSTableViewDataSource

extension CGFXModelsController: NSTableViewDataSource {

    func numberOfRows(in tableView: NSTableView) -> Int {
        guard let cgfx = cgfx else {
            return 0
        }

        return Int(cgfx.num_models)
    }

    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        guard let cgfx = cgfx, let model = cgfx.models[row]?.pointee, let cName = model.name else {
            return nil
        }

        let name = String(cString: cName)
        return name
    }
}

// MARK: - NSTableViewDelegate

extension CGFXModelsController: NSTableViewDelegate {

    func tableViewSelectionDidChange(_ notification: Notification) {
        // remove the old model
        modelNode?.removeFromParentNode()
        modelNode = nil

        guard let cgfx = cgfx, let selectedModel = selectedModel else {
            return
        }

        // load and display the model
        var model = Model(backing: selectedModel)
        let node = model.getNode(from: cgfx.file)
        scene.rootNode.addChildNode(node)
        modelNode = node
    }
}
