//
//  SPRController.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import Cocoa

/// The controller used for displaying SPRs.
class SPRController: NSViewController {

    // MARK: - Private Properties

    private let nameColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "NameColumn")
    private let formatColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "FormatColumn")

    // MARK: - Public Properties

    /// The SPR that this controller is currently displaying, if any.
    var spr: spr_t? {
        didSet {
            tableView.reloadData()
        }
    }

    // MARK: - Outlets

    @IBOutlet private weak var tableView: NSTableView!
}

// MARK: - NSTableViewDataSource

extension SPRController: NSTableViewDataSource {

    func numberOfRows(in tableView: NSTableView) -> Int {
        guard let spr = spr else {
            return 0
        }

        return Int(spr.num_ctpks)
    }

    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        guard let spr = spr else {
            return nil
        }

        let identifier = tableColumn!.identifier
        switch identifier {
        case nameColumnIdentifier:
            guard let cName = spr.ctpk_names[row] else {
                return nil
            }

            let name = String(cString: cName)
            return name
        case formatColumnIdentifier:
            // mirai ctpks only ever use one texture as images are handled at the spr level
            guard let texture = spr.ctpks[row]?.pointee.textures[0]?.pointee else {
                return nil
            }

            let format = Texture.Format(rawValue: texture.data_format.rawValue)!
            return format.name
        default:
            fatalError("unknown spr table column: \(identifier)")
        }
    }
}
