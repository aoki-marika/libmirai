//
//  CGFXController.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import Cocoa

/// The controller used for displaying CGFXs.
class CGFXController: NSTabViewController {

    // MARK: - Public Properties

    /// The CGFX that this controller is currently displaying, if any.
    var cgfx: cgfx_t? {
        didSet {
            if tabViewItems[selectedTabViewItemIndex] == texturesItem {
                guard let controller = texturesItem.viewController as? CGFXTexturesController else {
                    fatalError("unable to get cgfx textures controller")
                }

                controller.cgfx = cgfx
            }
        }
    }

    // MARK: - Outlets

    @IBOutlet private weak var texturesItem: NSTabViewItem!
}
