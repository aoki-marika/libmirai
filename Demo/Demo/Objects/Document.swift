//
//  Document.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import AppKit

/// The document for Mirai files.
///
/// Due to Mirai using the `.bin` extension for all of it's files, they all need to be wrapped in one document class,
/// and then have the contents read based on the signature.
class Document: NSDocument {

    // MARK: - Private Properties

    private let sprWindowControllerIdentifier = NSStoryboard.SceneIdentifier("SPRWindowController")
    private let aetWindowControllerIdentifier = NSStoryboard.SceneIdentifier("AETWindowController")
    private let cgfxWindowControllerIdentifier = NSStoryboard.SceneIdentifier("CGFXWindowController")

    /// The current contents of this document.
    private var contents = Contents.none

    // MARK: - Public Methods

    override func makeWindowControllers() {
        let storyboard = NSStoryboard(name: NSStoryboard.Name("Main"), bundle: nil)

        // create the window controller and pass down the document contents to its content
        switch contents {
        case .none:
            fatalError("attempted to open none contents")
        case .spr(let spr):
            let windowController = storyboard.instantiateController(withIdentifier: sprWindowControllerIdentifier) as! NSWindowController
            let controller = windowController.contentViewController as! SPRController
            controller.spr = spr
            self.addWindowController(windowController)
        case .cgfx(let cgfx):
            let windowController = storyboard.instantiateController(withIdentifier: cgfxWindowControllerIdentifier) as! NSWindowController
            let controller = windowController.contentViewController as! CGFXController
            controller.cgfx = cgfx
            self.addWindowController(windowController)
        case .aet(let aet):
            let windowController = storyboard.instantiateController(withIdentifier: aetWindowControllerIdentifier) as! NSWindowController
            let controller = windowController.contentViewController as! AETController
            controller.aet = aet
            self.addWindowController(windowController)
        }
    }

    override func read(from data: Data, ofType typeName: String) throws {
        // get the file path
        // this is needed as libmirai takes paths, not data objects
        guard let path = fileURL?.path.cString(using: .utf8) else {
            throw DocumentError.noPath
        }

        // determine the file contents based on the signature
        var signatureBytes = [UInt8](repeating: 0, count: 4)
        data.copyBytes(to: &signatureBytes, count: signatureBytes.count)

        switch signatureBytes {
        case [0x00, 0x00, 0x00, 0x00]:
            var spr = spr_t()
            spr_open(path, &spr)
            contents = .spr(spr)
        case [0x10, 0x00, 0x00, 0x00], [0x20, 0x00, 0x00, 0x00]:
            var aet = aet_t()
            aet_open(path, &aet)
            contents = .aet(aet)
        // CGFX ascii
        case [0x43, 0x47, 0x46, 0x58]:
            var cgfx = cgfx_t()
            cgfx_open(path, &cgfx)
            contents = .cgfx(cgfx)
        default:
            throw DocumentError.unknownFormat
        }
    }

    override func close() {
        switch contents {
        case .none:
            break
        case .spr(var spr):
            spr_close(&spr)
        case .cgfx(var cgfx):
            cgfx_close(&cgfx)
        case .aet(var aet):
            aet_close(&aet)
        }
    }
}

// MARK: - Contents

extension Document {
    /// The different contents that can be within a document.
    enum Contents {

        // MARK: - Cases

        /// This document contains nothing, only used as a placeholder.
        case none

        /// This document contains the given SPR.
        case spr(spr_t)

        /// This document contains the given CGFX.
        case cgfx(cgfx_t)

        /// This document contains the given AET.
        case aet(aet_t)
    }
}

// MARK: - DocumentError

/// The different errors that can occur while handling a document.
fileprivate enum DocumentError: Error {

    // MARK: - Cases

    /// The document was unable to find the path of the file being opened.
    case noPath

    /// The document attempted to open a file that was in an unknown format.
    case unknownFormat
}
