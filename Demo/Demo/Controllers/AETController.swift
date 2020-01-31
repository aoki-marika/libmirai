//
//  AETController.swift
//  Demo
//
//  Created by Marika on 2020-01-31.
//  Copyright © 2020 Marika. All rights reserved.
//

import Cocoa

class AETController: NSViewController {

    // MARK: - Public Properties

    /// The AET that this controller is currently displaying, if any.
    var aet: aet_t? {
        didSet {
            print(aet!)
        }
    }
}
