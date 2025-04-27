//
//  ContentView.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        TabView {
            NavigationView {
                SearchView()
            }.tabItem { Label("Meals", systemImage: "list.bullet.rectangle.portrait.fill") }
            
            NavigationView {
                FavoriteView()
            }.tabItem { Label("Favorite meals", systemImage: "heart.fill") }
            
            NavigationView {
                FavoriteView()
            }.tabItem { Label("This week", systemImage: "fork.knife") }
            
            NavigationView {
                ListView()
            }.tabItem { Label("List", systemImage: "basket.fill") }
        }

    }
}

#Preview {
    ContentView()
}
