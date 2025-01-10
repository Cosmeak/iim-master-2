//
//  WeekRecipeManager.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//
import Foundation

class WeekMealManager {
    private let weekMealsKey: String = "weekMeals"
    private var weekMeals: [String] = []
    
    init() {
        self.weekMeals = UserDefaults.standard.stringArray(forKey: weekMealsKey) ?? []
    }
    
    func getWeekMeals() -> [String] {
        return weekMeals
    }
    
    func addWeekMeal(id: String) -> [String] {
        if (!weekMeals.contains(id)) {
            weekMeals.append(id)
        }
        
        return weekMeals
    }
    
    func removeWeekMeal(id: String) {
        // TODO
    }
}
