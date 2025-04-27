//
//  FavoriteView.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import SwiftUI

struct FavoriteView: View {
    @StateObject private var viewModel = FavoriteViewModel()
    
    var body: some View {
        NavigationStack {
            VStack {
                if viewModel.meals.isEmpty {
                    Text("No favorites")
                        .font(.system(size: 21, weight: .semibold))
                        .foregroundStyle(Color.gray)
                } else {
                    ScrollView {
                        LazyVStack(spacing: 10) {
                            ForEach(viewModel.meals, id: \.id) { meal in
                                NavigationLink(destination: MealDetailView(meal: meal)) {
                                    MealCell(meal: meal)
                                        .padding(.horizontal)
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
