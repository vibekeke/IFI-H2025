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
        return self._contains(self.root, value)


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
        self.root = self._remove(self.root, value)

    def _remove(self, node, value): 
        if node is None:    #Node we want to delete not found
            return None     
        
        #Searching for the node to delete: 
        if value < node.value:
            node.left = self._remove(node.left, value)
            return node #Node should return itself so it can be attatched to its parent in the prior call :)
        if value > node.value:
            node.right = self._remove(node.right, value)
            return node
        #Else, aka value == node.value
        #If we got this far, the node we want to delete is the current one.


        #Scenario 1: The node we want to delete has 0-1 children
        #Replace self with one of its children (which can also None)
        if node.left is None:   
            self.size -= 1
            return node.right   #Replace the node with it's right child (or None if it's Node.right)
        if node.right is None:
            self.size -= 1
            return node.left    #Replace the node with it's left child
        
        #Scenario 2: The node we want to delete has 2 children:
        #Replace self with smallest successor of the right child.
        u = self._findMin(node.right)
        node.value = u.value
        node.right = self._remove(node.right, u.value)  #Now we need to remove that initial child.
        return node


        """
        Longer explanation of the remove() recursive function:
        INITIAL CALL

        We call the function as root = remove(root, value)
        Hence, the goal is to return the node which will be the NEW node.
        If the current node is NOT the thing we want to remove, the current node is just sent back as is.
        If the current node IS the thing we want to remove, we need to SEND THE RIGHT REPLACEMENT CHILD instead of itself
        That's the basic node.


        ALGORITM:

        Base case: If we reach a node that is None, we have reached the end of a branch without finding our node.

        SEARCHING:
            If the value is smaller than the node:
                Keep traversing the tree with v.left = _remove(v.left, value)
                Remember to return ITSELF, because this is not the node we want replaced.

        If the current node is neither smaller nor larger, we have found the one we want to delete.
        That means we want to return a child of this node as a replacement for itself.

        SCENARIO 1: 0 or 1 child
        if node.left is None, return node.right - node.right can be None too, and then we just return None.
        if node.left is NOT none, then we need to figure out if node.right is none. If right is None, return left.

        Scenario 2: 2 children
        if the node we want to delete has two children then we have to find
        node.rights smallest child. It can be itself.
        
        We use a helperfunction to find the smallest node.

        Then we replace the value of our current node (to make reparenting easy)

        Then finally, we delete the smallest child to avoid duplicates. 
        It is a leaf-node so using the ._remove() will clear that up fine.

        Finally, return the "new" self to reparent itself in the tree.

        """

    def _findMin(self, node):
        if node.left is None:
            return node
        return self._findMin(node.left)
    
    def size(self):
        return self.size

