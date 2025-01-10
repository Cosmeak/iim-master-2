//
//  MealDataSource.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation



class MealDataSource {
    static func searchMeal(searchedMeal: String) async throws -> MealResponseDTO {
        let url = URL(string: "https://www.themealdb.com/api/json/v1/1/search.php?s=\(searchedMeal)")
        let (data, _) = try await URLSession.shared.data(from: url!)
        
        return try JSONDecoder().decode(MealResponseDTO.self, from: data)
    }
    
    static func lookupMeal(id: String) async throws -> MealResponseDTO {
        let url = URL(string: "httms://www.themealdb.com/api/json/v1/1/lookup.php?i=\(id)")
        let (data, _) = try await URLSession.shared.data(from: url!)
        
        return try JSONDecoder().decode(MealResponseDTO.self, from: data)
    }
    
    static func getCategories() async throws -> CategoryResponseDTO {
        let url = URL(string: "https://www.themealdb.com/api/json/v1/1/categories.php")
        let (data, _) = try await URLSession.shared.data(from: url!)
        
        return try JSONDecoder().decode(CategoryResponseDTO.self, from: data)
    }
}
