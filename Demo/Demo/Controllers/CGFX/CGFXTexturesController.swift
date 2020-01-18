//
//  CGFXTexturesController.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import Cocoa

/// The controller for displaying textures within a CGFX.
class CGFXTexturesController: NSViewController {

    // MARK: - Private Properties

    private let nameColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "NameColumn")
    private let sizeColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "SizeColumn")
    private let formatColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "FormatColumn")

    /// The currently selected texture in this controller, if any.
    private var selectedTexture: texture_t? {
        let selectedRow = tableView.selectedRow
        guard selectedRow >= 0 else {
            return nil
        }

        return cgfx?.textures[selectedRow]?.pointee.texture
    }

    // MARK: - Public Properties

    /// The CGFX that this controller is currently displaying the texture of, if any.
    var cgfx: cgfx_t? {
        didSet {
            tableView.reloadData()
        }
    }

    // MARK: - Outlets

    @IBOutlet private weak var tableView: NSTableView!
    @IBOutlet private weak var textureView: NSImageView!
}

// MARK: - NSTableViewDataSource

extension CGFXTexturesController: NSTableViewDataSource {

    func numberOfRows(in tableView: NSTableView) -> Int {
        guard let cgfx = cgfx else {
            return 0
        }

        return Int(cgfx.num_textures)
    }

    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        guard let cgfx = cgfx, let txob = cgfx.textures[row]?.pointee else {
            return nil
        }

        let texture = txob.texture
        let identifier = tableColumn!.identifier
        switch identifier {
        case nameColumnIdentifier:
            guard let cName = txob.name else {
                return nil
            }

            let name = String(cString: cName)
            return name
        case sizeColumnIdentifier:
            let size = "\(texture.width)x\(texture.height)"
            return size
        case formatColumnIdentifier:
            let format = Texture.Format(rawValue: texture.data_format.rawValue)!
            return format.name
        default:
            fatalError("unknown cgfx texture table column: \(identifier)")
        }
    }
}

// MARK: - NSTableViewDelegate

extension CGFXTexturesController: NSTableViewDelegate {

    func tableViewSelectionDidChange(_ notification: Notification) {
        guard let cgfx = cgfx, let selectedTexture = selectedTexture else {
            textureView.image = nil
            return
        }

        // load and display the texture
        var texture = Texture(backing: selectedTexture)
        textureView.image = texture.getImage(from: cgfx.file)
    }
}
