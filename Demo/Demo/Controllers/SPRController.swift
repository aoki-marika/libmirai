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

            let formatName: String
            switch texture.data_format {
            case TEXTURE_FORMAT_RGBA8888: formatName = "RGBA8888"
            case TEXTURE_FORMAT_RGB888:   formatName = "RGB888"
            case TEXTURE_FORMAT_RGBA5551: formatName = "RGBA5551"
            case TEXTURE_FORMAT_RGB565:   formatName = "RGB565"
            case TEXTURE_FORMAT_RGBA4444: formatName = "RGBA4444"
            case TEXTURE_FORMAT_LA88:     formatName = "LA88"
            case TEXTURE_FORMAT_HL8:      formatName = "HL8"
            case TEXTURE_FORMAT_L8:       formatName = "L8"
            case TEXTURE_FORMAT_A8:       formatName = "A8"
            case TEXTURE_FORMAT_LA44:     formatName = "LA44"
            case TEXTURE_FORMAT_L4:       formatName = "L4"
            case TEXTURE_FORMAT_A4:       formatName = "A4"
            case TEXTURE_FORMAT_ETC1:     formatName = "ETC1"
            case TEXTURE_FORMAT_ETC1_A4:  formatName = "ETC1 A4"
            default: fatalError()
            }

            return formatName
        default:
            fatalError("unknown spr table column: \(identifier)")
        }
    }
}
