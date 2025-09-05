
class Node:
    def __init__(self, key):
        self.key = key
        self.right = None
        self.left = None
        self.height = 0


class AVL:
    def __init__(self):
        self.root = None

    def height(self, node):                                     # Fra slide 2
        if node == None:
            return -1
        else:
            return node.height

    def SetHeight(self, node):
        if node != None:
            node.height = 1 + max(self.height(node.left), self.height(node.right)) 


    def getBalanceFactor(self, v):                              # Fra slides 8
        if v == None:
            return 0
        return self.height(v.left) - self.height(v.right)

    def leftRotate(self, z):                                    # Fra slides 4
        y = z.right
        T1 = y.left
        
        y.left = z
        z.right = T1

        self.SetHeight(z)
        self.SetHeight(y)

        return y

    def rightRotate(self, z):                                   # Fra slides 5
        y = z.left
        T2 = y.right
        
        y.right = z
        z.left = T2

        self.SetHeight(z)
        self.SetHeight(y)

        return y

    def balanceTree(self, v):                                   # Fra slides 9

        if self.getBalanceFactor(v) < -1:
            if self.getBalanceFactor(v.right) > 0:
                v.right = self.rightRotate(v.right)
            return self.leftRotate(v)
        
        if self.getBalanceFactor(v) > 1:
            if self.getBalanceFactor(v.left) < 0:
                v.left = self.leftRotate(v.left)
            return self.rightRotate(v)
        
        return v

    def insert(self, v, x):                                 # Fra slide 10
        if v == None:
            v = Node(x)
        elif x < v.key:
            v.left = self.insert(v.left, x)
        elif x > v.key:
            v.right = self.insert(v.right, x)
        self.SetHeight(v)
        return self.balanceTree(v)



    def remove(self, v, x):
        if v == None:
            return None
        if x < v.key:
            v.left = self.remove(v.left,x)
        elif x > v.key:
            v.right = self.remove(v.right,x)
        elif v.left == None:
            v = v.right
        elif v.right == None:
            v = v.left
        else:
            u = self.findMin(v.right)  
            v.key = u.key
            v.right = self.remove(v.right, u.key)
        self.SetHeight(v)
        return self.balanceTree(v)

    def findMin(self, node):
        if node == None:
            return None
        
        if node.left != None:
            return self.findMin(node.left)
        
        return node
        
