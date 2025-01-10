//
//  SearchView.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import SwiftUI

struct SearchView: View {
    @StateObject private var viewModel = SearchViewModel()
    
    var body: some View {
        NavigationStack {
            VStack {
                if viewModel.meals.isEmpty {
                    Text("No results")
                        .font(.system(size: 21, weight: .semibold))
                        .foregroundStyle(Color.gray)
                } else {
                    ScrollView {
                        LazyVStack(spacing: 10) {
                            ForEach(viewModel.meals, id: \.id) { meal in
                                NavigationLink(destination: RecipeDetailView(meal: meal)) {
                                    RecipeCell(meal: meal)
                                        .padding(.horizontal)
                                }
                            }
                        }
                    }
                }
            }
            .navigationBarTitleDisplayMode(.inline)
            .searchable(text: $viewModel.searchedMeal, prompt: "Search for a recipe...")
        }
    }
}
