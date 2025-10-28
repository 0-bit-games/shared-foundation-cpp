// swift-tools-version:5.9

//
// Package.swift
// foundation
//
// Created by Kristian Trenskow on 2023/10/07
// See license in LICENSE.
//

import PackageDescription

let package = Package(
	name: "SharedFoundationCxx",
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
