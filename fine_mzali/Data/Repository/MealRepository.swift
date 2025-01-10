//
//  MealRepository.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

class MealRepository {
    func searchMeal(searchedMeal: String) async throws-> [Meal] {
        let response = try await MealDataSource.searchMeal(searchedMeal: searchedMeal)
        return MealMapper.mapResponseToModels(input: response.meals)
    }
    
    func lookupMeal(id: String) async throws -> Meal? {
        let response = try await MealDataSource.lookupMeal(id: id)
        return MealMapper.mapResponseToModels(input: response.meals).first
    }
}
