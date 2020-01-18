//
//  CGFXWindowController.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import Cocoa

/// The controller for the window of a CGFX controller.
class CGFXWindowController: NSWindowController {

    // MARK: - Outlets

    @IBOutlet private weak var viewControl: NSSegmentedControl!

    // MARK: - Public Methods

    override func windowDidLoad() {
        super.windowDidLoad()
        updateTab()
    }

    // MARK: - Private Methods

    /// Update the currently selected tab of this controller's content controller.
    private func updateTab() {
        guard let tabController = contentViewController as? NSTabViewController else {
            return
        }

        tabController.selectedTabViewItemIndex = viewControl.indexOfSelectedItem
    }

    // MARK: - Actions

    @IBAction func viewChanged(_ sender: Any) {
        updateTab()
    }
}
