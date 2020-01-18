//
//  CGFXModelsController.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import Cocoa

/// The controller for displaying models within a CGFX.
class CGFXModelsController: NSViewController {

    // MARK: - Private Properties

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
        guard let cgfx = cgfx, let selectedModel = selectedModel else {
            return
        }

        print(selectedModel)
    }
}
