
class Node:
    def __init__(self, key):
        self.key = key
        self.right = None
        self.left = None
        self.height = 0


class AVL:
    def __init__(self):
        self.root = None

    def height(self, node):
        if node is None:
            return -1
        else:
            return node.height

    def setHeight(self, node):
        if node != None:
            node.height = 1 + max(self.height(node.left), self.height(node.right))

    def getBalanceFactor(self, v):
        if v = null:
            return 0
        #Take the left childs height and the right sides height, and compare
        #Positive number if left is bigger, negative if right is bigger. 
        return self.height(v.left), - self.height(v.right)

    def leftRotate(self, z):
        #We want reparent z by making z a child of its right child y.

        #Step 1: Name some nodes, because we only know of z so far
        y = z.right
        T1 = y.left

        #Step 2: Rotate
        y.left = z
        z.right = T1

        #Step 3: Set new heights
        #MERK rekkefølgen betyr noe her, siden høyde avhenger av hverandre
        self.setHeight(y.left) #z
        self.setHeight(y)

        return y #To set as the root node


    def rightRotate(self, z):
        y = z.left
        T2 = y.right

        y.right = z
        z.left = T2
        
        self.setHeight(y.right) #z
        self.setHeight(y)

        return y

    def balanceTree(self, v):
        balanceFactor = v.getBalanceFactor

        if balanceFactor == 0:
            return
        if balanceFactor > 0:   #Treet lener mot venstre
            v = self.balanceTree(v.left)
        elif balanceFactor < 0:
            v = self.balanceTree(v.right)
        


    def insert(self, v, x):
        #Først helt standard innsetting
        if v = null:
            v = Node(x)
        elif x < v.element:
            v.left = self.insert(v.left, x)
        elif x > v.element:
            v.right = self.insert(v.right, x)
        #Deretter balansering
        self.setHeight(v)
        return self.balanceTree(v) #Kan ende opp med ny rot, derfor returnerer vi


    def remove(self, v, x):
        #Først helt standard fjerning
        if v is None:
            return None
        
        #Step 0: Searching for the correct node to delete.
        if x < v.element:
            v.left = self.remove(v.left, x)
            #no return statement cause we don't know what to parent yet
        elif x > v.element:
            v.right = self.remove(v.right, x)
        #If we got to this elif, that means x == v.element, and this node is what we wanna delete
        #Scenario 1:
        elif v.left is None:
            v = v.right
        elif v.right is None:
            v = v.left
        else:   #Scenario 2, aka. 2 children
            u = self.findMin(v.right)
            v.element = u.element
            v.right = self.remove(v.right, u.element)
        
        #Sjekk om subtreet nå er tomt:
        if v is None:
            return None

        #Deretter balansering
        self.setHeight(v)
        return self.balanceTree(v)

    def findMin(self, node):
        if node.left = None:
            return node
        return self.findMin(node.left)


