import random

def sort(A): #Hovedfunksjon, tar inn et array
    n = len(A) #Definerer lengden på arrayet
    quicksort(A,0,n-1) #Kaller på den rekursive funksjonen
    return A #Returnerer et sortert array

def quicksort(A,low,high): #Rekursiv funksjon
    if low >= high:
        return A
    p = partition(A, low, high)
    quicksort(A, low, p - 1)
    quicksort(A, p + 1, high)
    return A

def partition(A, low, high): #Flytter elementer mindre enn pivoten til venstre og elementer større enn pivoten til høyre
    p = choosePivot(A, low, high)
    A.swap(p, high)

    pivot = A[high]
    left = low
    right = high - 1

    while left <= right:
        while left <= right and A[left] <= pivot:
            left = left + 1
        while right >= left and A[right] >= pivot:
            right = right - 1
        if left < right:
            A.swap(left, right)
    A.swap(left, high)
    return left

def choosePivot(A,low,high): #Denne velger pivot i arrayet tilfeldig
    return random.randint(low,high)
