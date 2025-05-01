#  Fonction d'aide pour afficher l'état actuel des cadres (pour le débogage)
def afficher_cadres(cadres)
  if cadres.empty?
    puts "Cadres: Vide"
  else
    puts "Cadres: #{cadres.map { |f| f.nil? ? '-' : f }.join(' | ')}"
  end
end

# -------------------- Algorithme FIFO --------------------
def simuler_fifo(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = [] # Initialise les cadres comme un tableau vide
  defauts_page = 0
  index_cadre = 0

  puts "\n--- Simulation FIFO ---"

  chaine_reference.each do |page|
    if !cadres.include?(page)
      puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      if cadres.length < taille_memoire
        cadres << page # Ajoute la page au premier cadre disponible
      else
        cadres[index_cadre] = page # Remplace la page la plus ancienne
        index_cadre = (index_cadre + 1) % taille_memoire
      end
    else
      puts "La page #{page} est déjà en mémoire."
    end
    afficher_cadres(cadres)
  end

  donnees[:resultats_algorithmes][:fifo] = { defauts_page: defauts_page }
  puts "Total Défauts de Page FIFO: #{defauts_page}"
end

# -------------------- Algorithme Optimal --------------------
def simuler_optimal(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = []
  defauts_page = 0

  puts "\n--- Simulation Optimal ---"

  chaine_reference.each_with_index do |page, index|
    if !cadres.include?(page)
      puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      if cadres.length < taille_memoire
        cadres << page
      else
        # Trouve la page dans les cadres qui ne sera pas utilisée le plus longtemps dans le futur
        utilisation_plus_lointaine = -1
        index_remplacement = -1
        cadres.each_with_index do |p, i|
          prochain_index_utilisation = chaine_reference[index + 1..-1].index(p)
          if prochain_index_utilisation.nil?
            index_remplacement = i
            break # Aucune utilisation future, remplace immédiatement
          elsif prochain_index_utilisation > utilisation_plus_lointaine
            utilisation_plus_lointaine = prochain_index_utilisation
            index_remplacement = i
          end
        end
        cadres[index_remplacement] = page
      end
    else
      puts "La page #{page} est déjà en mémoire."
    end
    afficher_cadres(cadres)
  end

  donnees[:resultats_algorithmes][:optimal] = { defauts_page: defauts_page }
  puts "Total Défauts de Page Optimal: #{defauts_page}"
end

# -------------------- Algorithme LRU --------------------
def simuler_lru(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = []
  defauts_page = 0
  utilisé_recemment = {}

  puts "\n--- Simulation LRU ---"

  chaine_reference.each_with_index do |page, index|
    if !cadres.include?(page)
      puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      if cadres.length < taille_memoire
        cadres << page
      else
        # Trouve la page la moins récemment utilisée
        page_lru = cadres.min_by { |p| utilisé_recemment[p] || 0 } # Utilise 0 pour les pages non utilisées
        cadres[cadres.index(page_lru)] = page
      end
    else
      puts "La page #{page} est déjà en mémoire."
    end
    afficher_cadres(cadres)
    utilisé_recemment[page] = index # Met à jour l'utilisation récente pour la page actuelle
  end

  donnees[:resultats_algorithmes][:lru] = { defauts_page: defauts_page }
  puts "Total Défauts de Page LRU: #{defauts_page}"
end

# -------------------- Algorithme de la Seconde Chance --------------------
def simuler_seconde_chance(donnees)
  chaine_reference = donnees[:chaine_reference]
  taille_memoire = donnees[:taille_memoire]
  cadres = []
  bits_reference = [] # Tableau pour les bits de référence
  defauts_page = 0
  index_cadre = 0

  puts "\n--- Simulation Seconde Chance ---"

  chaine_reference.each do |page|
    if !cadres.include?(page)
      puts "La page #{page} a causé un défaut de page."
      defauts_page += 1
      if cadres.length < taille_memoire
        cadres << page
        bits_reference << 1 # Ajoute le bit de référence
      else
        while true
          if bits_reference[index_cadre] == 0
            cadres[index_cadre] = page
            bits_reference[index_cadre] = 1
            index_cadre = (index_cadre + 1) % taille_memoire
            break
          else
            bits_reference[index_cadre] = 0 # Donne une seconde chance
            index_cadre = (index_cadre + 1) % taille_memoire
          end
        end
      end
    else
      puts "La page #{page} est déjà en mémoire."
      bits_reference[cadres.index(page)] = 1
    end
    afficher_cadres(cadres.zip(bits_reference).map { |p, r| p.nil? ? '-' : "#{p}(#{r})" })
  end

  donnees[:resultats_algorithmes][:seconde_chance] = { defauts_page: defauts_page }
  puts "Total Défauts de Page Seconde Chance: #{defauts_page}"
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