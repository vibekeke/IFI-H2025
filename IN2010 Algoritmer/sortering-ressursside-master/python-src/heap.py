def sort(A): #Hovedmetode, tar inn et array
    n = len(A) #definerer n til buildMaxHeap
    buildMaxHeap(A,n)
    for i in range(n-1, 0, -1): #Går igjennom hele arrayet
        A.swap(0,i)
        BubbleDown(A,0,i)
    return A #returnerer et array

def buildMaxHeap(A,n):
    for i in range (n//2-1, -1, -1): #Definerer i til BubbleDown
        BubbleDown(A,i,n)

def BubbleDown(A,i,n): #Lager en heap
    largest = i
    left = (2*i) + 1
    right = (2*i)+2

    if left < n and A[largest] < A[left]:  #finner det største elementet og plasserer det øverst
        largest = left

    if right < n and A[largest] < A[right]:
        largest = right

    if i != largest:
        A.swap(i,largest)
        BubbleDown(A, largest, n)
