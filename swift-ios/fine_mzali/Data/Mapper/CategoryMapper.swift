//
//  CategoryMapper.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

class CategoryMapper {
    static func mapResponseToModels(input response: [CategoryEntryDTO]) -> [Category] {
        return response.map { result in
            return Category(
                id: result.idCategory,
                name: result.strCategory,
                thumbnail: result.strCategoryThumb,
                description: result.strCategoryDescription
            )
        }
    }
}
