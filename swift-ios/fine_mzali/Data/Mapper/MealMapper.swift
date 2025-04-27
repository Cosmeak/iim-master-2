//
//  MealMapper.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

class MealMapper {
    static func mapResponseToModels(input response: [MealEntryDTO]) -> [Meal] {
        return response.map { result in
            let tags: [String] = result.strTags?.components(separatedBy: ",") ?? []
            var ingredients: [MealIngredient] = []
            
            for i in 1...20 {
                let ingredientName: String? = result["strIngredient\(i)"]
                if (ingredientName != nil) {
                    ingredients.append(MealIngredient(name: ingredientName!, measure: result["strMeasure\(i)"]))
                }
            }
            
            return Meal(id: result.idMeal, name: result.strMeal, area: result.strArea, tags: tags, instruction: result.strInstructions, thumbnail: result.strMealThumb, ingredients: ingredients)
        }
    }
}
