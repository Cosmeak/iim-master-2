//
//  CategoryDTO.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

struct CategoryResponseDTO: Decodable {
    let categories: [CategoryEntryDTO]
}

struct CategoryEntryDTO: Decodable {
    let idCategory: String
    let strCategory: String
    let strCategoryThumb: String
    let strCategoryDescription: String
}
