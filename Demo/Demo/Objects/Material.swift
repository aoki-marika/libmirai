//
//  Material.swift
//  Demo
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

import AppKit
import SceneKit

/// A data structure to wrap around `mtob_t` and provide SceneKit data types.
struct Material {

    // MARK: - Private Properties

    /// The backing MTOB of this material.
    private let backing: mtob_t

    // MARK: - Initializers

    /// - Parameter backing: The backing MTOB of this material.
    init(backing: mtob_t) {
        self.backing = backing
    }

    // MARK: - Public Methods

    /// Get the SceneKit representation of this material's backing.
    /// - Returns: The SceneKit representation of this material's backing.
    func getMaterial() -> SCNMaterial {
        let material = SCNMaterial()
        return material
    }
}

// MARK: - NSColor Extensions

extension NSColor {
    convenience init(_ color: color4_t) {
        self.init(
            red: CGFloat(color.r),
            green: CGFloat(color.g),
            blue: CGFloat(color.b),
            alpha: CGFloat(color.a)
        )
    }
}
