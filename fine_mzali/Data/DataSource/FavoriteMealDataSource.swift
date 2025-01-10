//
//  FavManager.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

class FavoriteMealDataSource {
    private let favoriteMealskey: String = "favoriteMeals"
    private var favoritesMeals: [String]
    
    init() {
        self.favoritesMeals = UserDefaults.standard.stringArray(forKey: favoriteMealskey) ?? []
    }
    
    func addFavoriteMeal(id: String) -> [String] {
        if (favoritesMeals.contains(id) == false) {
            favoritesMeals.append(id)
        }
        
        UserDefaults.standard.set(favoritesMeals, forKey: favoriteMealskey)
        return favoritesMeals
    }
    
    func getFavoriteMeals() -> [String] {
        return favoritesMeals
    }
}
