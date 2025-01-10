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
    
    func getFavoritesMeals() async throws -> [Meal] {
        let favoritesMealsIds = FavoriteMealDataSource().getFavoriteMeals()
        var loadedMeals: [Meal] = []
        
        for id in favoritesMealsIds {
            let meal = try await self.lookupMeal(id: id)
            if meal != nil {
                loadedMeals.append(meal!)
            }
        }
        
        return loadedMeals
    }
    
    func addFavoriteMeal(id: String) async throws -> [Meal] {
        let _ = FavoriteMealDataSource().addFavoriteMeal(id: id)
        return try await self.getFavoritesMeals()
    }
    
    func getWeekMeals() async throws -> [Meal] {
        let weekMealsIds = WeekMealManager().getWeekMeals()
        var loadedMeals: [Meal] = []
        
        for id in weekMealsIds {
            let meal = try await self.lookupMeal(id: id)
            if meal != nil {
                loadedMeals.append(meal!)
            }
        }
        
        return loadedMeals
    }
    
    func addWeekMeal(id: String) async throws -> [Meal] {
        let _ = WeekMealManager().addWeekMeal(id: id)
        return try await self.getWeekMeals()
    }
    
    func getShoppingList() async throws -> [String: String] {
        let meals = try await self.getWeekMeals()
        var shoppingList: [String: String] = [:]
        
        for meal in meals {
            for ingredient in meal.ingredients {
                let measure = shoppingList[ingredient.name]
                if (measure != nil) {
                    shoppingList[ingredient.name] = "\(measure!) \(ingredient.measure ?? "")"
                } else {
                    shoppingList[ingredient.name] = ingredient.measure ?? ""
                }
                
            }
        }
        
        return shoppingList
    }
}
