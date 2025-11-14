def sort(A):
    n = len(A)                                      ## Sjekker lengden til listen og lagrer den i n
    for i in range (1 , n):                         ## Går gjennom alle tallene i listen
        j = i                                       ## Lagrer hvor vi er i listen
        while j > 0 and A[j - 1] > A[j]:            ## While løkke som vil kjøre fram til tallet vårt er sortert er eller vi er på slutten av listen (A[j-i] > A[j] hvis tallet vårt er er mindre en tallet en posisjon bak så bytter vi plass)
            A.swap(j-1, j)                          ## Bruker Swap funksjon som ligger i skjellet for å ting skal telles
            j -= 1                                      
    return A                                        

