//
//  RecipeView.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import SwiftUI

struct MealCell: View {
    let meal: Meal

    var body: some View {
        HStack(spacing: 10) {
            if let url = URL(string: meal.thumbnail) {
                AsyncImage(url: url) { image in
                    image
                        .resizable()
                        .scaledToFill()
                } placeholder: {
                    Color.gray.opacity(0.3)
                }
                .frame(width: 60, height: 60)
                .clipShape(Circle())
                .shadow(radius: 3)
            }
            
            VStack(alignment: .leading, spacing: 5) {
                Text(meal.name)
                    .font(.headline)
                    .foregroundColor(.primary)
                    .lineLimit(1)
                
                HStack(spacing: 5) {
                    Image(systemName: "mappin.and.ellipse")
                        .foregroundColor(.gray)
                        .font(.subheadline)
                    Text(meal.area)
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                        .lineLimit(1)
                }
            }
            Spacer()
        }
        .padding(.vertical, 8)
        .padding(.horizontal, 10)
    }
}
