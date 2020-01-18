//
//  Texture.swift
//  Demo
//
//  Created by Marika on 2020-01-18.
//  Copyright © 2020 Marika. All rights reserved.
//

import AppKit

/// A data structure to wrap around `texture_t` and provide AppKit data types.
struct Texture {

    // MARK: - Private Properties

    /// The backing texture of this texture.
    private var backing: texture_t

    // MARK: - Public Properties

    /// The format of this texture's backing's data.
    let format: Format

    // MARK: - Initializers

    /// - Parameter backing: The backing texture of this texture.
    init(backing: texture_t) {
        self.backing = backing
        self.format = Format(rawValue: backing.data_format.rawValue)!
    }

    // MARK: - Public Methods

    /// Get the `NSImage` representation of this texture's backing, if any.
    /// - Parameter file: The file to read this texture's backing data from.
    /// - Returns: The `NSImage` representation of this texture's backing, if any.
    mutating func getImage(from file: UnsafeMutablePointer<FILE>) -> NSImage? {
        // decode the data to a coregraphics data provider
        guard let cData = texture_decode(&backing, file) else {
            return nil
        }

        guard let data = CFDataCreate(kCFAllocatorDefault, cData, backing.decoded_data_size) else {
            return nil
        }

        guard let dataProvider = CGDataProvider(data: data) else {
            return nil
        }

        // get the data parameters depending on the format
        let width = Int(backing.width)
        let height = Int(backing.height)
        let format = Format(rawValue: backing.data_format.rawValue)!

        let bitsPerComponent: Int
        let bitsPerPixel: Int
        let bitmapInfo: CGBitmapInfo
        let colorSpace: CGColorSpace
        switch format {
        case .rgba8888:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent * 4
            bitmapInfo = [CGBitmapInfo(rawValue: CGImageAlphaInfo.last.rawValue)]
            colorSpace = CGColorSpaceCreateDeviceRGB()
        case .rgb888:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent * 3
            bitmapInfo = []
            colorSpace = CGColorSpaceCreateDeviceRGB()
        // coregraphics seems provide no way to read packed data
        // but these formats are never used in mirai so skip them
        case .rgba5551, .rgb565:
            return nil
        case .rgba4444:
            bitsPerComponent = 4
            bitsPerPixel = bitsPerComponent * 4
            bitmapInfo = [CGBitmapInfo(rawValue: CGImageAlphaInfo.last.rawValue)]
            colorSpace = CGColorSpaceCreateDeviceRGB()
        case .la88:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent * 2
            bitmapInfo = [CGBitmapInfo(rawValue: CGImageAlphaInfo.last.rawValue)]
            colorSpace = CGColorSpaceCreateDeviceGray()
        case .hl8:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent * 2
            bitmapInfo = []
            colorSpace = CGColorSpaceCreateDeviceRGB()
        case .l8:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent
            bitmapInfo = []
            colorSpace = CGColorSpaceCreateDeviceGray()
        case .a8:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent
            bitmapInfo = [CGBitmapInfo(rawValue: CGImageAlphaInfo.alphaOnly.rawValue)]
            colorSpace = CGColorSpaceCreateDeviceGray()
        case .la44:
            bitsPerComponent = 4
            bitsPerPixel = bitsPerComponent * 2
            bitmapInfo = [CGBitmapInfo(rawValue: CGImageAlphaInfo.last.rawValue)]
            colorSpace = CGColorSpaceCreateDeviceGray()
        case .l4:
            bitsPerComponent = 4
            bitsPerPixel = bitsPerComponent
            bitmapInfo = []
            colorSpace = CGColorSpaceCreateDeviceGray()
        case .a4:
            bitsPerComponent = 4
            bitsPerPixel = bitsPerComponent
            bitmapInfo = [CGBitmapInfo(rawValue: CGImageAlphaInfo.alphaOnly.rawValue)]
            colorSpace = CGColorSpaceCreateDeviceGray()
        case .etc1:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent * 3
            bitmapInfo = []
            colorSpace = CGColorSpaceCreateDeviceRGB()
        case .etc1a4:
            bitsPerComponent = 8
            bitsPerPixel = bitsPerComponent * 4
            bitmapInfo = [CGBitmapInfo(rawValue: CGImageAlphaInfo.last.rawValue)]
            colorSpace = CGColorSpaceCreateDeviceRGB()
        }

        let bytesPerRow = Int(ceil(Float(bitsPerPixel) / 8)) * width

        // create the coregraphics image
        guard let cgImage = CGImage(
            width: width,
            height: height,
            bitsPerComponent: bitsPerComponent,
            bitsPerPixel: bitsPerPixel,
            bytesPerRow: bytesPerRow,
            space: colorSpace,
            bitmapInfo: bitmapInfo,
            provider: dataProvider,
            decode: nil,
            shouldInterpolate: false,
            intent: .defaultIntent
        ) else {
            return nil
        }

        // deallocate the data now that its read
        cData.deallocate()

        // create and return the appkit image
        let size = CGSize(width: width, height: height)
        return NSImage(cgImage: cgImage, size: size)
    }
}

// MARK: - Format

extension Texture {
    /// The different formats that a texture's data can be in.
    ///
    /// This is only a Swift wrapper around `enum texture_format_t`, see that for case documentation.
    enum Format: UInt32 {

        // MARK: - Cases

        case rgba8888 = 0x0
        case rgb888   = 0x1
        case rgba5551 = 0x2
        case rgb565   = 0x3
        case rgba4444 = 0x4
        case la88     = 0x5
        case hl8      = 0x6
        case l8       = 0x7
        case a8       = 0x8
        case la44     = 0x9
        case l4       = 0xa
        case a4       = 0xb
        case etc1     = 0xc
        case etc1a4   = 0xd

        // MARK: - Public Properties

        /// The display name of this format.
        var name: String {
            switch self {
            case .rgba8888: return "RGBA8888"
            case .rgb888:   return "RGB888"
            case .rgba5551: return "RGBA5551"
            case .rgb565:   return "RGB565"
            case .rgba4444: return "RGBA4444"
            case .la88:     return "LA88"
            case .hl8:      return "HL8"
            case .l8:       return "L8"
            case .a8:       return "A8"
            case .la44:     return "LA44"
            case .l4:       return "L4"
            case .a4:       return "A4"
            case .etc1:     return "ETC1"
            case .etc1a4:   return "ETC1 A4"
            }
        }
    }
}
