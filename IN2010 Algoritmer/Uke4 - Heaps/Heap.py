#I en heap ønsker vi å legge til på bunnen, siden vi har en referanse til siste node, og så boble opp? 
#Ved sletting ønsker vi å ta noden vi ønsker å slette, og bytte plass med den siste noden i treet - og så boble ned hvis nødvendig...


class Heap:
    def __init__(self):
        self.heap = list()

    def bubbleDown(self, i):
        H = self.heap
        j = leftOf(i)

        while j < len(H):
            r = rightOf(i)

            #If the right child-index is even in the list...
            #And the right child is smaller than the left child...
            if r < len(H) and H[r] < H[j]:
                j = r   #The one we want to swap with is the right child, not the left
            if H[i] <= H[j]:
                return  #The tree is now correct, and we can end our operation
            #Tuple swap
            H[i], H[j] = H[j], H[i]
            i = j
            j = leftOf(i)

    def bubbleUp(self, i):
        #Assigning shorthands, not strictly necessary
        H = self.heap
        p = parentOf(i)

        while 0 < i and H[i] < H[p]:
            #Swap places
            H[i], H[p] = H[p], H[i]

            #Assign next index
            i = parentOf(i)
            p = parentOf(i)

    def insert(self, value):
        self.heap.append(value)
        self.bubbleUp(len(self.heap)-1)

    def remove(self):
        if not self.heap:
            return

        removed = self.heap[0]
        self.heap[0] = self.heap[-1]
        self.heap.pop() #Removing the last element, ignore the 'pop' word, it works different with arrays and heaps
        if len(self.heap) > 1: #Because we used a helper function this extra thing is necessary I guess?
            self.bubbleDown(0)
        return removed

    
# Valgfrie hjelpemetoder
def parentOf(i):
    return (i - 1) // 2 #Gir -1 hvis det er roten av treet, siden roten av treet har index 0

def leftOf(i):
    return (2 * i) + 1 

def rightOf(i):
    return (2 * i) + 2


class Node():
    def __init__(self, s, f, left, right):
        self.symbol = s
        self.freq = f
        self.left = left
        self.right = right

def build_hofftree(priority_queue):
    Q = Heap() #Empty heap
    C = priority_queue #Heap instance full of tuples
    for s, f in C:
        Q.insert(Node(s, f, None, None)) 
            #We pretend we have a class node with children
            #We also pretend our methods above can compare the nodes using f
    while len(Q.heap) > 1:
            #Remove the two smallest
        v1 = Q.remove()
        V2 = Q.remove()
        f = v1.freq + v2.freq
        Q.insert(Node(None, f, v1, v2))
    return Q.remove()

'''

Input: A root node, HoffTree with tuples (s, f)
Output: Leaf nodes with tuples (s, f) where s != null

proc HoffmanLeaves(v, C):
    if v == null:
        return

    v = hoffmanLeaves

'''




def main():
    #Skriv noen tester her!
    # Sett for eksempel inn 10 verdier, og sjekk at verdien som hentes ut er det du forventer!
    pass

if __name__ == "__main__":
    main()
