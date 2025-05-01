require 'matplotlib/pyplot'
# Fonction pour simuler l'algorithme de remplacement de page FIFO
def fifo(pages, capacity, frame_contents, page_faults)
  page_faults[0] = 0
  frame = Array.new(capacity, -1) # Initialise le frame comme étant vide
  frame_index = 0

  pages.each do |page|
    page_hit = false
    (0...capacity).each do |j|
      if frame[j] == page
        page_hit = true
        break
      end
    end
    if !page_hit
      page_faults[0] += 1
      frame[frame_index] = page
      frame_index = (frame_index + 1) % capacity # Buffer circulaire
    end
    #pour le débogage
    #puts "Frame: #{frame.join(" ")}"
  end
  return 0
end

# Fonction pour simuler l'algorithme de remplacement de page Optimal
def optimal(pages, capacity, frame_contents, page_faults)
  page_faults[0] = 0
  frame = Array.new(capacity, -1) # Initialise le frame comme étant vide

  (0...pages.size).each do |i|
    page_hit = false
    (0...capacity).each do |j|
      if frame[j] == pages[i]
        page_hit = true
        break
      end
    end
    if !page_hit
      page_faults[0] += 1
      farthest = -1
      replace_index = -1
      (0...capacity).each do |j|
        k = (i + 1...pages.size).find { |k| frame[j] == pages[k] }
        if k
          if k > farthest
            farthest = k
            replace_index = j
          end
        else
          replace_index = j
          break
        end
      end
      replace_index = 0 if replace_index == -1
      frame[replace_index] = pages[i]
    end
    #pour le débogage
    #puts "Frame: #{frame.join(" ")}"
  end
  return 0
end

# Fonction pour simuler l'algorithme de remplacement de page LRU
def lru(pages, capacity, frame_contents, page_faults)
  page_faults[0] = 0
  frame = Array.new(capacity, -1) # Initialise le frame comme étant vide
  time = Array.new(capacity, 0) # Garde la trace du moment où chaque page a été utilisée pour la dernière fois
  current_time = 0

  (0...pages.size).each do |i|
    page_hit = false
    (0...capacity).each do |j|
      if frame[j] == pages[i]
        page_hit = true
        time[j] = current_time
        current_time += 1 # Met à jour le temps de la dernière utilisation
        break
      end
    end
    if !page_hit
      page_faults[0] += 1
      lru_index = (0...capacity).min_by { |j| time[j] }
      frame[lru_index] = pages[i]
      time[lru_index] = current_time
      current_time += 1
    end
    #pour le débogage
    #puts "Frame: #{frame.join(" ")}"
  end
  return 0
end

# Fonction pour simuler l'algorithme de remplacement de page Second Chance
def second_chance(pages, capacity, frame_contents, page_faults)
  page_faults[0] = 0
  frame = Array.new(capacity, -1) # Initialise le frame comme étant vide
  referenced = Array.new(capacity, false) # Garde la trace du bit de référence
  frame_index = 0

  pages.each do |page|
    page_hit = false
    (0...capacity).each do |j|
      if frame[j] == page
        page_hit = true
        referenced[j] = true # Met le bit de référence
        break
      end
    end
    if !page_hit
      page_faults[0] += 1
      while true
        if !referenced[frame_index]
          frame[frame_index] = page
          referenced[frame_index] = true # Met le bit de référence pour la nouvelle page
          frame_index = (frame_index + 1) % capacity
          break
        else
          referenced[frame_index] = false # Efface le bit de référence
          frame_index = (frame_index + 1) % capacity
        end
      end
    end
    #pour le débogage
    #puts "Frame: #{frame.join(" ")}"
  end
  return 0
end

# Fonction pour obtenir l'entrée de l'utilisateur pour les références de page et la taille du frame
def get_user_input(pages, n, capacity)
  puts "1. Pour saisir manuellement les données"
  puts "2. Pour charger les données à partir d'un fichier texte"
  puts "3. Pour revenir au menu principal"
  print "Entrez votre choix : "
  choice = gets.chomp.to_i

  case choice
  when 1
    print "Entrez le nombre de références de pages : "
    n[0] = gets.chomp.to_i
    pages.clear
    print "Entrez les références de pages (séparées par des espaces) : "
    page_input = gets.chomp.split.map(&:to_i)
    pages.concat(page_input)
    print "Entrez le nombre de pages de la mémoire tampon : "
    capacity[0] = gets.chomp.to_i
  when 2
    print "Entrez le nom du fichier texte : "
    filename = gets.chomp
    begin
      file = File.open(filename, "r")
      # D'abord, lire le nombre de pages
      n[0] = file.readline.chomp.to_i
      # Ensuite, lire les références de page
      pages.clear
      (0...n[0]).each do |i|
        pages << file.readline.chomp.to_i
      end
      # Enfin, lire la capacité
      capacity[0] = file.readline.chomp.to_i
      file.close
    rescue Errno::ENOENT
      puts "Erreur lors de l'ouverture du fichier : #{filename}"
      return -1
    rescue => e
      puts "Erreur lors de la lecture du fichier : #{e.message}"
      return -1
    end
  when 3
    return 1 # Retour au menu principal
  else
    puts "Choix invalide."
    return -1
  end
  return 0
end

# Fonction pour afficher les résultats de la simulation
def display_results(algorithm_name, page_faults)
  puts "Algorithme #{algorithm_name}: #{page_faults[0]} fautes de page"
end

# Fonction pour sauvegarder les résultats dans un fichier
def save_results(algorithm_name, page_faults, filename)
  File.open(filename, "a") do |file| # Ajouter au fichier
    file.puts "Algorithme #{algorithm_name} | #{page_faults[0]}"
  end
rescue
  puts "Erreur lors de l'ouverture du fichier"
end

# Fonction pour afficher les résultats sauvegardés à partir du fichier
def display_saved_results(filename)
  begin
    file = File.open(filename, "r")
    puts "\n--- Résultats des Exécutions Précédentes ---"
    file.each_line do |line|
      puts line.chomp # Affiche chaque ligne du fichier
    end
    file.close
  rescue Errno::ENOENT
    puts "Erreur lors de l'ouverture du fichier"
  rescue => e
    puts "Erreur lors de la lecture du fichier: #{e.message}"
  end
end

# Fonction pour afficher la simulation graphiquement.
def display_simulation(pages, capacity, frame_contents, algorithm_name)
    page_faults = [0]
    frame = Array.new(capacity, -1)

    if algorithm_name == "FIFO"
        fifo(pages, capacity, frame_contents, page_faults)
    elsif algorithm_name == "Optimal"
        optimal(pages, capacity, frame_contents, page_faults)
    elsif algorithm_name == "LRU"
        lru(pages, capacity, frame_contents, page_faults)
    elsif algorithm_name == "Second Chance"
        second_chance(pages, capacity, frame_contents, page_faults)
    end

    print "\nRéférences de pages logiques: "
    puts pages.join(" ")
    puts "\n"

    print "Pages physiques -->\n"
    (0...pages.size).each do |i|
        page_hit = false
        (0...capacity).each do |j|
            if frame[j] == pages[i]
                page_hit = true
                break
            end
        end
        if !page_hit
            print "F "
        else
            print "  "
        end
    end
    puts "\n"
    print "Pages logiques -->\n"
    (0...pages.size).each { print "  " }
    puts "\n"

    if algorithm_name == "Second Chance"
        frame2 = Array.new(capacity, -1)
        referenced = Array.new(capacity, false)
        frame_index = 0
        (0...pages.size).each do |i|
            page_hit = false
            (0...capacity).each do |j|
                if frame2[j] == pages[i]
                    page_hit = true
                    referenced[j] = true
                    break
                end
            end
            if !page_hit
                while true
                    if !referenced[frame_index]
                        frame2[frame_index] = pages[i]
                        referenced[frame_index] = true
                        frame_index = (frame_index + 1) % capacity
                        break
                    else
                        referenced[frame_index] = false
                        frame_index = (frame_index + 1) % capacity
                    end
                end
            end
            (0...capacity).each do |j|
                if frame2[j] == -1
                    print "  "
                else
                    print "#{frame2[j]}#{referenced[j] ? '+' : '-'} "
                end
            end
            puts
        end
    else
        (0...pages.size).each do |i|
            page_hit = false
            (0...capacity).each do |j|
                if frame[j] == pages[i]
                    page_hit = true
                    break
                end
            end
            if !page_hit
                print "  "
            else
                (0...capacity).each do |j|
                  if frame[j] == -1
                    print "  "
                  else
                    print "#{frame[j]} "
                  end
                end
                puts
            end
        end
    end
end

def main
  pages = [] #  références de pages
  n = [0]           # Nombre de références de pages
  capacity = [0]    # Taille du frame de la mémoire
  frame_contents = []
  # Efface le fichier de résultats au début de chaque exécution.
  File.open("if3602.txt", "w") {}
  # Initialize Matplotlib
  Matplotlib.use(:Agg)
  begin
    loop do
      puts "\nMenu TP IF3602-2024/2025 (Réalisateur: Nom et Prénom de chaque membre)"
      puts "Veuillez choisir l'option à exécuter :"
      puts "1. Chargement (fichier texte) ou saisie des données"
      puts "2. Simulation avec Algorithme FIFO"
      puts "3. Simulation avec Algorithme optimal"
      puts "4. Simulation avec Algorithme LRU"
      puts "5. Simulation avec la seconde chance"
      puts "6. Affichage des Exécutions Effectuées."
      puts "7. Graphiques des algorithmes simulés"
      puts "8. Quitter l'Application."
      print "Entrez votre choix : "
      choice = gets.chomp.to_i

      case choice
      when 1
        if get_user_input(pages, n, capacity) == -1
          # Gérer l'erreur ou retourner au menu principal
          break
        end
      when 2
        page_faults = [0]
        fifo(pages, capacity[0], frame_contents, page_faults)
        display_results("FIFO", page_faults[0])
        save_results("FIFO", page_faults[0], "if3602.txt")
        display_simulation(pages, capacity[0], frame_contents, "FIFO")
      when 3
        page_faults = [0]
        optimal(pages, capacity[0], frame_contents, page_faults)
        display_results("Optimal", page_faults[0])
        save_results("Optimal", page_faults[0], "if3602.txt")
        display_simulation(pages, capacity[0], frame_contents, "Optimal")
      when 4
        page_faults = [0]
        lru(pages, capacity[0], frame_contents, page_faults)
        display_results("LRU", page_faults[0])
        save_results("LRU", page_faults[0], "if3602.txt")
        display_simulation(pages, capacity[0], frame_contents, "LRU")
      when 5
        page_faults = [0]
        second_chance(pages, capacity[0], frame_contents, page_faults)
        display_results("Second Chance", page_faults[0])
        save_results("Second Chance", page_faults[0], "if3602.txt")
        display_simulation(pages, capacity[0], frame_contents, "Second Chance")
      when 6
        display_saved_results("if3602.txt")
      when 7
        # Générer le graphique
        generate_graph(pages, capacity[0])
      when 8
        puts "Exiting application."
        break
      else
        puts "Choix invalide. Veuillez réessayer."
      end
    end
  rescue Interrupt
    puts "\nInterrupted by user. Exiting."
  rescue => e
    puts "An unexpected error occurred: #{e}"
  end
end

def generate_graph(pages, capacity)
    frame_contents = []
    page_faults_fifo = [0]
    fifo(pages, capacity, frame_contents, page_faults_fifo)

    page_faults_optimal = [0]
    optimal(pages, capacity, frame_contents, page_faults_optimal)

    page_faults_lru = [0]
    lru(pages, capacity, frame_contents, page_faults_lru)

    page_faults_second_chance = [0]
    second_chance(pages, capacity, frame_contents, page_faults_second_chance)
  
    # Créer le graphique
    Matplotlib.pyplot.plot(
        [1, 2, 3, 4],
        [page_faults_fifo[0], page_faults_optimal[0], page_faults_lru[0], page_faults_second_chance[0]],
        marker: 'o',
        linestyle: '-',
        color: 'b',
        label: 'Fautes de Page'
    )
    
    # Ajouter des labels et un titre
    Matplotlib.pyplot.xlabel('Algorithmes')
    Matplotlib.pyplot.ylabel('Nombre de Fautes de Page')
    Matplotlib.pyplot.title('Comparaison des Algorithmes de Remplacement de Page')
    Matplotlib.pyplot.xticks([1, 2, 3, 4], ['FIFO', 'Optimal', 'LRU', 'Second Chance'])
    Matplotlib.pyplot.legend()
    Matplotlib.pyplot.grid(true)
  
    # Sauvegarder le graphique dans un fichier
    Matplotlib.pyplot.savefig('page_replacement_comparison.png')
  
    # Afficher le graphique
    Matplotlib.pyplot.show()
end

main
