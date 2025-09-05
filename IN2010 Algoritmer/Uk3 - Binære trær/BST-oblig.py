class Node:                         # Hjelpe klasse for node
    self.value
    self.left = None
    self.right = None

    def __init__(self, value):
        self.value = value

class BST:
    self.root = None
    self.size = 0


    def contains(self, value):
        if self.root is None:
            return
        
        return self._contains(self.root)


    def _contains(self, node, value):
        if node is None:
            return False
        
        if value == node.value:
            return True
        elif value < node.value:
            return self._contains(node.left, value)
        elif value > node.value:
            return self._contains(node.right, value)
            
            
        
    def insert(self, value):
        if self.root is None:
            self.root = Node(value)
            self.size += 1
            return
        
        self._insert(self.root, value)
        
    def _insert(self, node, value):
        if node is None:
            v = Node(value)
            self.size += 1
            return v
    
        if value < node.value:
            node.left = self._insert(node, value)
        elif value > self.value: 
            node.right = self._insert(node, value)
        
        return node #Important to return the node so the tree don't break

    def remove(self, value):
        if self.root is None:
            return #nothing to delete.
        
        self._remove(self.root, value)

    def _remove(self, v, value): 
        if node is None:
            return None

        

    def _findMin(self, node):
    
    def size(self):

