class Node:
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None

class AVL:
    def __init__(self):
        self.root = None

    def height(self, root): #-1 if empty.
        if root = None:
            return -1

        #Note that the null entry at the end of a leaf will give -1.
        return 1 + max(self.height(self.left), self.height(self.right))


    def getBalanceFactor(self, root):
        pass

    def leftRotate(self, root):
        pass

    def rightRotate(self, root):
        pass

    def balanceTree(self, root):
        pass

    def insert(self, value):
        pass

    def delete(self, value):
        pass

    def traverse(self, root):
        pass


def main():
    tree = AVL()

    tree.insert(1)
    assert tree.height(tree.root) == 0

    tree.insert(2)
    tree.insert(3)
    assert tree.height(tree.root) == 1

    tree.insert(4)
    tree.insert(5)
    tree.insert(6)
    tree.insert(7)
    assert tree.height(tree.root) == 2

    tree.insert(8)
    tree.insert(9)
    tree.insert(10)
    tree.insert(11)
    tree.insert(12)
    tree.insert(13)
    tree.insert(14)
    tree.insert(15)
    assert tree.height(tree.root) == 3

    tree.insert(16)
    assert tree.height(tree.root) == 4

    print("Hurra! Testene er passert")
    
if __name__ == "__main__":
   main()