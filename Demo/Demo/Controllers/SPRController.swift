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
    private let sizeColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "SizeColumn")
    private let formatColumnIdentifier = NSUserInterfaceItemIdentifier(rawValue: "FormatColumn")

    /// The currently selected texture in this controller, if any.
    private var selectedTexture: texture_t? {
        let selectedRow = tableView.selectedRow
        guard selectedRow >= 0 else {
            return nil
        }

        return spr?.textures[selectedRow]
    }

    // MARK: - Public Properties

    /// The SPR that this controller is currently displaying, if any.
    var spr: spr_t? {
        didSet {
            tableView.reloadData()
        }
    }

    // MARK: - Outlets

    @IBOutlet private weak var tableView: NSTableView!
    @IBOutlet private weak var textureView: NSImageView!
    @IBOutlet private weak var scrsView: NSImageView!

    // MARK: - Public Methods

    override func keyDown(with event: NSEvent) {
        // toggle the scrs view if h is pressed
        if event.keyCode == 4 {
            scrsView.isHidden = !scrsView.isHidden
        }
        else {
            super.keyDown(with: event)
        }
    }
}

// MARK: - NSTableViewDataSource

extension SPRController: NSTableViewDataSource {

    func numberOfRows(in tableView: NSTableView) -> Int {
        guard let spr = spr else {
            return 0
        }

        return Int(spr.num_textures)
    }

    func tableView(_ tableView: NSTableView, objectValueFor tableColumn: NSTableColumn?, row: Int) -> Any? {
        guard let spr = spr else {
            return nil
        }

        let identifier = tableColumn!.identifier
        switch identifier {
        case nameColumnIdentifier:
            guard let cName = spr.texture_names[row] else {
                return nil
            }

            let name = String(cString: cName)
            return name
        case sizeColumnIdentifier:
            let texture = spr.textures[row]
            let size = "\(texture.width)x\(texture.height)"
            return size
        case formatColumnIdentifier:
            let texture = spr.textures[row]
            let format = Texture.Format(rawValue: texture.data_format.rawValue)!
            return format.name
        default:
            fatalError("unknown spr table column: \(identifier)")
        }
    }
}

// MARK: - NSTableViewDelegate

extension SPRController: NSTableViewDelegate {

    func tableViewSelectionDidChange(_ notification: Notification) {
        guard let spr = spr, let selectedTexture = selectedTexture else {
            textureView.image = nil
            scrsView.image = nil
            return
        }

        // load and display the texture
        var texture = Texture(backing: selectedTexture)
        textureView.image = texture.getImage(from: spr.file)

        // load the scrs into a separate overlay image
        // this is done so they can be toggled off and on
        let w = CGFloat(selectedTexture.width)
        let h = CGFloat(selectedTexture.height)
        let size = NSSize(width: w, height: h)
        let scrsImage = NSImage(size: size)
        scrsImage.lockFocus()

        for index in 0..<Int(spr.num_scrs) {
            let scr = spr.scrs[index]

            // ensure the scr is for the selected texture
            guard scr.ctpk_index == tableView.selectedRow else {
                continue
            }

            // get the corners of the scr
            // scrs are top left origin, while cg is bottom left
            // so convert from top to bottom for the y positions
            // as nsbezierpath draws paths centered on the points,
            // 0.5 is added/subtracted to ensure that the 1px line is
            // drawn on the pixel grid, not between it
            let topLeft = CGPoint(x: (CGFloat(scr.top_left.x) * w) + 0.5, y: (CGFloat(1.0 - scr.top_left.y) * h) - 0.5)
            let topRight = CGPoint(x: (CGFloat(scr.bottom_right.x) * w) - 0.5, y: (CGFloat(1.0 - scr.top_left.y) * h) - 0.5)
            let bottomLeft = CGPoint(x: (CGFloat(scr.top_left.x) * w) + 0.5, y: (CGFloat(1.0 - scr.bottom_right.y) * h) + 0.5)
            let bottomRight = CGPoint(x: (CGFloat(scr.bottom_right.x) * w) - 0.5, y: (CGFloat(1.0 - scr.bottom_right.y) * h) + 0.5)

            // create the outline path for the scr
            let path = NSBezierPath()
            path.move(to: topLeft)
            path.line(to: topRight)
            path.line(to: bottomRight)
            path.line(to: bottomLeft)
            path.close()

            // draw the scr outline
            NSColor.controlAccentColor.set()
            path.stroke()

            // draw the name
            let name = String(cString: scr.name)
            NSString(string: name).draw(
                in: NSRect(
                    x: bottomLeft.x,
                    y: bottomLeft.y,
                    width: topRight.x - topLeft.x,
                    height: topLeft.y - bottomLeft.y
                ),
                withAttributes: [
                    .font: NSFont.labelFont(ofSize: NSFont.labelFontSize),
                    .foregroundColor: NSColor.controlAccentColor,
                    .backgroundColor: NSColor.black,
                ]
            )
        }

        scrsImage.unlockFocus()
        scrsView.image = scrsImage
    }
}
