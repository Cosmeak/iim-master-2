//
//  MealDetailViewModel.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

@MainActor
class MealDetailViewModel: ObservableObject {
    let repository = MealRepository()
    
    func addToFavorite(id: String) -> Void {
        Task {
            let _ = try await repository.addFavoriteMeal(id: id)
        }
    }
    
    func isFavorite() -> Bool {
        return false
    }
}
