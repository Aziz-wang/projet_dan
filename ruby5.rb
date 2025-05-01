#  Helper function to display the formatted output
def afficher_simulation(chaine_reference, cadres_historique, defauts_page_positions)
  puts "références de pages logiques"
  puts chaine_reference.join(" ")  # Display page references with spaces

  puts " " * chaine_reference.length * 2 # Add spacing for alignment

  puts " " # Add a newline for spacing

  puts "pages physiques -> pages logiques"

  # Display the frames at each step
  cadres_historique.each_with_index do |cadres, index|
    print "---"
  end
  puts ""

  cadres_historique.each_with_index do |cadres, index|
    if cadres.empty?
      print "|" + " " * 3 + "|"
    else
      max_height = cadres_historique.map(&:length).max
      (0...max_height).each do |i|
        if cadres[i]
          print "|" + cadres[i].to_s.center(3) + "|"
        else
          print "|" + " " * 3 + "|"
        end
        puts ""
      end
    end
    puts ""
  end

  puts "\nTotal Défauts de Page: #{defauts_page_positions.count}"
end

# -------------------- Algorithme FIFO --------------------
def simuler_fifo(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = []
  cadres_historique = [] # Store frame states at each step
  defauts_page_positions = [] # Store indices of page faults

  defauts_page = 0
  index_cadre = 0

  puts "\n--- Simulation FIFO ---"

  chaine_reference.each_with_index do |page, index|
    if cadres.include?(page)
      #puts "La page #{page} est déjà en mémoire."
    else
      #puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      defauts_page_positions << index
      if cadres.length < taille_memoire
        cadres << page
      else
        cadres[index_cadre] = page
        index_cadre = (index_cadre + 1) % taille_memoire
      end
    end
    cadres_historique << cadres.dup # Store a copy of the current frame state
  end

  donnees[:algorithm_results][:fifo] = { defauts_page: defauts_page }
  afficher_simulation(chaine_reference, cadres_historique, defauts_page_positions)
end

# -------------------- Algorithme Optimal --------------------
def simuler_optimal(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = []
  cadres_historique = []
  defauts_page_positions = []

  defauts_page = 0

  puts "\n--- Simulation Optimal ---"

  chaine_reference.each_with_index do |page, index|
    if cadres.include?(page)
      #puts "La page #{page} est déjà en mémoire."
    else
      #puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      defauts_page_positions << index
      if cadres.length < taille_memoire
        cadres << page
      else
        utilisation_plus_lointaine = -1
        index_remplacement = -1
        cadres.each_with_index do |p, i|
          prochain_index_utilisation = chaine_reference[index + 1..-1].index(p)
          if prochain_index_utilisation.nil?
            index_remplacement = i
            break
          elsif prochain_index_utilisation > utilisation_plus_lointaine
            utilisation_plus_lointaine = prochain_index_utilisation
            index_remplacement = i
          end
        end
        cadres[index_remplacement] = page
      end
    end
    cadres_historique << cadres.dup
  end

  donnees[:algorithm_results][:optimal] = { defauts_page: defauts_page }
  afficher_simulation(chaine_reference, cadres_historique, defauts_page_positions)
end

# -------------------- Algorithme LRU --------------------
def simuler_lru(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = []
  cadres_historique = []
  defauts_page_positions = []

  defauts_page = 0
  utilisé_recemment = {}

  puts "\n--- Simulation LRU ---"

  chaine_reference.each_with_index do |page, index|
    if cadres.include?(page)
      #puts "La page #{page} est déjà en mémoire."
    else
      #puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      defauts_page_positions << index
      if cadres.length < taille_memoire
        cadres << page
      else
        page_lru = cadres.min_by { |p| utilisé_recemment[p] || 0 }
        cadres[cadres.index(page_lru)] = page
      end
    end
    cadres_historique << cadres.dup
    utilisé_recemment[page] = index
  end

  donnees[:algorithm_results][:lru] = { defauts_page: defauts_page }
  afficher_simulation(chaine_reference, cadres_historique, defauts_page_positions)
end

# -------------------- Algorithme de la Seconde Chance --------------------
def simuler_seconde_chance(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = []
  cadres_historique = []
  defauts_page_positions = []

  bits_reference = []
  defauts_page = 0
  index_cadre = 0

  puts "\n--- Simulation Seconde Chance ---"

  chaine_reference.each_with_index do |page, index|
    if cadres.include?(page)
      #puts "La page #{page} est déjà en mémoire."
      bits_reference[cadres.index(page)] = 1
    else
      #puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      defauts_page_positions << index
      if cadres.length < taille_memoire
        cadres << page
        bits_reference << 1
      else
        while true
          if bits_reference[index_cadre] == 0
            cadres[index_cadre] = page
            bits_reference[index_cadre] = 1
            index_cadre = (index_cadre + 1) % taille_memoire
            break
          else
            bits_reference[index_cadre] = 0
            index_cadre = (index_cadre + 1) % taille_memoire
          end
        end
      end
    end
    cadres_historique << cadres.dup
    bits_reference.length.times { |i| bits_reference[i] = 0 if cadres[i] != chaine_reference[index]}
  end

  donnees[:algorithm_results][:seconde_chance] = { defauts_page: defauts_page }
  afficher_simulation(chaine_reference, cadres_historique, defauts_page_positions)
end

def afficher_menu
  puts "Menu TP IF3602-2024/2025 (Réalisateur: Nom et Prénom de chaque membre)"
  puts "Veuillez choisir l'option à exécuter :"
  puts "1. Chargement (fichier texte) ou saisie des données"
  puts "2. Simulation avec Algorithme FIFO"
  puts "3. Simulation avec Algorithme optimal"
  puts "4. Simulation avec Algorithme LRU"
  puts "5. Simulation avec la seconde chance"
  puts "6. Affichage des Exécutions Effectuées."
  puts "7. Graphiques des algorithmes simulés"
  puts "8. Quitter l'Application."
end

def obtenir_choix_utilisateur
  print "Entrez votre choix : "
  gets.chomp.to_i
end

def obtenir_donnees(donnees)
  loop do
    puts "1. Pour saisir manuellement les données"
    puts "2. Pour charger les données à partir d'un fichier texte"
    puts "3. Pour revenir au menu principal"
    choix = obtenir_choix_utilisateur

    case choix
    when 1
      saisir_donnees_manuellement(donnees)
      break
    when 2
      lire_donnees_depuis_fichier(donnees)
      break
    when 3
      break
    else
      puts "Choix invalide."
    end
  end
end

def saisir_donnees_manuellement(donnees)
  print "Entrez la chaîne de référence des pages (par exemple, 7,0,1,2,0,3,0,4,2,3,0,3,2,1,2,0,1,7,0,1) : "
  donnees[:chaine_reference] = gets.chomp.split(',').map(&:to_i)

  print "Entrez le nombre de cadres de page : "
  donnees[:taille_memoire] = gets.chomp.to_i
end

def lire_donnees_depuis_fichier(donnees)
  print "Entrez le nom du fichier : "
  nom_fichier = gets.chomp
  begin
    lignes = File.readlines(nom_fichier)
    #  Parse the file to get reference_string and frame_size
    #  Error handling if the file format is incorrect
    donnees[:chaine_reference] = lignes[0].chomp.split(',').map(&:to_i)
    donnees[:taille_memoire] = lignes[1].chomp.to_i
  rescue Errno::ENOENT
    puts "Fichier non trouvé !"
  rescue StandardError => e
    puts "Une erreur s'est produite lors de la lecture du fichier : #{e.message}"
  end
end

def afficher_resultats(donnees)
  puts "\n--- Résultats des Simulations ---"
  donnees[:algorithm_results].each do |algorithme, resultat|
    puts "Algorithme : #{algorithme.to_s.upcase} | Défauts de Page : #{resultat[:defauts_page]}"
  end
end

def generer_graphiques(donnees)
  #  This part requires a graphing library like 'ruby2d' or 'gruff'
  #  Example using placeholders:
  puts "\n--- Génération des Graphiques (Non Implémenté) ---"
  puts "La génération de graphiques nécessite une bibliothèque graphique (ex: Ruby2D, Gruff)."
  puts "Vous devrez installer et intégrer une telle bibliothèque pour cette fonctionnalité."
end

def ecrire_resultats_dans_fichier(donnees, nom_fichier)
  File.open(nom_fichier, 'w') do |fichier|
    fichier.puts "--- Résultats des Simulations ---"
    donnees[:algorithm_results].each do |algorithme, resultat|
      fichier.puts "Algorithme : #{algorithme.to_s.upcase} | Défauts de Page : #{resultat[:defauts_page]}"
    end
  end
  puts "\nRésultats écrits dans le fichier #{nom_fichier}"
end

def menu_principal
  donnees = {
    chaine_reference: [],
    taille_memoire: 0,
    algorithm_results: {}
  }

  loop do
    afficher_menu
    choix = obtenir_choix_utilisateur

    case choix
    when 1
      obtenir_donnees(donnees)
    when 2
      simuler_fifo(donnees)
      donnees[:algorithm_results][:fifo] = { defauts_page: donnees[:algorithm_results][:fifo][:defauts_page] }
    when 3
      simuler_optimal(donnees)
      donnees[:algorithm_results][:optimal] = { defauts_page: donnees[:algorithm_results][:optimal][:defauts_page] }
    when 4
      simuler_lru(donnees)
      donnees[:algorithm_results][:lru] = { defauts_page: donnees[:algorithm_results][:lru][:defauts_page] }
    when 5
      simuler_seconde_chance(donnees)
      donnees[:algorithm_results][:seconde_chance] = { defauts_page: donnees[:algorithm_results][:seconde_chance][:defauts_page] }
    when 6
      afficher_resultats(donnees)
    when 7
      generer_graphiques(donnees)
    when 8
      puts "Fermeture de l'application."
      break
    else
      puts "Choix invalide. Veuillez réessayer."
    end
  end

  ecrire_resultats_dans_fichier(donnees, "if3602.txt")
end

#  Exécute le programme
menu_principal