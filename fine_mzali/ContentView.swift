//
//  ContentView.swift
//  fine_mzali
//
//  Created by Guillaume Fine on 10/01/2025.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        Group {
            TabView {
                NavigationView {
                    SearchView()
                }.tabItem { Label("Recipes", systemImage: "list.bullet.rectangle.portrait.fill") }
                
                
                NavigationView {
                    SearchView()
                }.tabItem { Label("This week", systemImage: "fork.knife") }
                
                NavigationView {
                    SearchView()
                }.tabItem { Label("List", systemImage: "basket.fill") }
            }
        }
    }
}

#Preview {
    ContentView()
}
