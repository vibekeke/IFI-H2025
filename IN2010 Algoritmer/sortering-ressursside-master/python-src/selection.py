def sort(A):
    n = len(A)
    for i in range (n-1):
        k = i
        for j in range(i + 1, n):
            if A[j] < A[k]:
                k = j
        if i != k:
            A.swap(i,k)
    return A                                        