//
//  RecipeDetailView.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import SwiftUI

struct MealDetailView: View {
    let meal: Meal
    @StateObject private var viewModel = MealDetailViewModel()
    @State private var isFavorite: Bool = false

    var body: some View {
        ScrollView(.vertical, showsIndicators: false) {
            VStack(spacing: 20) {
                VStack(alignment: .leading, spacing: 15) {
                    Text(meal.name)
                        .font(.system(size: 29, weight: .bold, design: .default))
                        .foregroundColor(.primary)
                    
                    if let url = URL(string: meal.thumbnail) {
                        AsyncImage(url: url) { image in
                            image
                                .resizable()
                                .scaledToFill()
                        } placeholder: {
                            Color.gray.opacity(0.3)
                        }
                        .frame(height: 250)
                        .clipped()
                        .cornerRadius(15)
                    }
                    
                    HStack(spacing: 10) {
                        if !meal.tags.isEmpty {
                            ForEach(meal.tags, id: \.self) { tag in
                                Text(tag)
                                    .font(.caption)
                                    .fontWeight(.semibold)
                                    .foregroundColor(.white)
                                    .padding(.vertical, 5)
                                    .padding(.horizontal, 10)
                                    .background(Color.blue)
                                    .cornerRadius(15)
                            }
                        }
                        
                        HStack(spacing: 8) {
                            Image(systemName: "mappin.and.ellipse")
                                .foregroundColor(.red)
                            Text(meal.area)
                                .font(.subheadline)
                                .foregroundColor(.secondary)
                            Button(action: {
                                print("meal added")
                                viewModel.addToFavorite(id: meal.id)
                            }) {
                                Text("Ajouter au favoris")
                            }
                        }
                    }
                    Divider()
                        .background(Color.gray)
                    
                    Text(meal.instruction)
                        .font(.body)
                        .foregroundColor(.primary)
                        .multilineTextAlignment(.leading)
                }
                .padding()
            }
            .padding()
        }
        .background(Color(.systemBackground))
        .navigationTitle("Recipe Details")
        .navigationBarTitleDisplayMode(.inline)
    }
}
