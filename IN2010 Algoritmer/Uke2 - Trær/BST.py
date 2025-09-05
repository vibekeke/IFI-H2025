class Node:
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None

class BST:
    def __init__(self):
        self.root = None

    def height(self, root):
        if root == None:
            return -1

        return 1 + max(self.height(root.left), self.height(root.right))

    def findMax(self, root):    #Returns value of max node.
        if root == None:
            return None

        maximum = root.value
    
        max_left = self.findMax(root.left)
        max_right = self.findMax(root.right)

        if max_left is not None:
            maximum = max(maximum, max_left)
        if max_right is not None:
            maximum = max(maximum, max_left)

        return maximum



    def findMax(self, root):    #Returns value of max node.
        if root == None:
            return None         #Or raise exception in a different language.

        if root.right != None:
            return self.findMax(root.right)
        
        return root.value

    def findMin(self, root):
        if root == None:
            return None         #Or raise exception in a different language.
        
        if root.left != None:
            return self.findMin(root.left)
        
        return root.value


    def insert(self, value):
        v = Node(value)

        if self.root == None:
            self.root = v
            return

        current = self.root
        parent = None

        while current != None:
            parent = current
            if value < current.value:
                current = parent.left
            elif value > current.value:
                current = parent.right
            else: 
                #Duplicate, no need to add anything
                return

        if value < parent.value:
            parent.left = v
        else:
            parent.right = v
    


    def contains(self, value):
        if self.root == None:
            return False
        
        current = self.root

        while current != None:
            if current.value == value:
                return True
            elif value < current.value:
                current = current.left
            elif value > current.value:
                current = current.right
    
        return False
            

    def delete(self, value):
        pass

    def traverse(self, root):
        pass


def main():
    tree = BST()

    tree.insert(8)
    tree.insert(4)
    tree.insert(10)
    tree.insert(7)
    tree.insert(9)

    assert tree.findMin(tree.root).value == 4
    assert tree.findMax(tree.root).value == 10

    tree._delete(10)
    tree.findMax(tree.root)

    assert tree._contains(10) == False
    assert tree._contains(7) == True

    tree._insert(1)
    tree._insert(2)
    tree._insert(3)
    assert tree.height(tree.root) == 4
    assert tree.findMin(tree.root).value == 1

    print("Hurra! Testene er passert")
    
if __name__ == "__main__":
   main()