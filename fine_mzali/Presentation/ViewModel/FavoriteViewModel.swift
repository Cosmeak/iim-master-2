//
//  FavoriteViewModel.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

@MainActor
class FavoriteViewModel: ObservableObject {
    let repository = MealRepository()
    @Published var meals: [Meal] = []
    
    init(){
        Task {
            do {
                self.meals = try await repository.getFavoritesMeals()
            } catch {
                print(error)
            }
        }
    }
}
