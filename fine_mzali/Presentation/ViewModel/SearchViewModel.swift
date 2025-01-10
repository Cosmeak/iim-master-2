//
//  SearchViewModel.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import Foundation

@MainActor
class SearchViewModel: ObservableObject {
    let repository = MealRepository()
    @Published var meals: [Meal] = []
    @Published var isLoading: Bool = false
    @Published var searchedMeal: String = "" {
        didSet { searchMeals() }
    }
    
    init() {
        searchMeals()
    }
    
    func searchMeals() {
        Task {
            isLoading = true
            do {
                meals = try await repository.searchMeal(searchedMeal: searchedMeal)
            } catch {
                print(error)
            }
            
            isLoading = false
        }
    }
}
