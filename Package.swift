// swift-tools-version:6.3

//
// Package.swift
// 0-bit-games-shared-foundation-cpp
//
// Created by Kristian Trenskow on 2023/10/07
// See license in LICENSE.
//

import PackageDescription

let package = Package(
	name: "SharedFoundationCxx",
	platforms: [.iOS(.v17), .macOS(.v14)],
	products: [
		.library(
			name: "SharedFoundationCxx",
			targets: ["SharedFoundationCxx"])
	],
	targets: [
		.target(
			name: "SharedFoundationCxx",
			path: ".",
			exclude: [
				"README.md",
				"LICENSE"
			],
			publicHeadersPath: "./include",
			swiftSettings: [
				.interoperabilityMode(.Cxx)
			]
		)
	],
	cxxLanguageStandard: .cxx20
)
