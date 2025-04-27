//
//  Meal.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

struct Meal {
    let id: String
    let name: String
    let area: String
    let tags: [String]
    let instruction: String
    let thumbnail: String
    let ingredients: [MealIngredient]
}

struct MealIngredient {
    let name: String
    let measure: String?
}
