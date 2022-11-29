import graphviz
import math
import typing


class Value:

    def __init__(self, data: typing.Union[float, int], children=None, op=None,
                 label="", backward=lambda: None):
        self.children = children if children is not None else ()
        self.op = op
        self.label = label
        self.grad = 0.0
        # _backward propagates gradients from parents to children.
        self._backward = lambda: None

        if isinstance(data, float):
            self.data = data
            return
        if isinstance(data, int):
            self.data = float(data)
            return

        raise TypeError("Values can only be floats or ints.")

    def __repr__(self):
        return f"Value(data={self.data})"
    
    def backward(self):
        nodes = []
        visited = set()
        def dfs(v):
            nodes.append(v)
            visited.add(v)
            for child in v.children:
                if child not in visited:
                    dfs(child)
        dfs(self)
        
        self.grad = 1
        for v in nodes:
            v._backward()

    def __add__(self, other):
        out = Value(self.data + other.data, children=(self, other), op="+")

        # _backward propagates the gradients to the children. Because of chain
        # rule. _backward() is always of the form:
        # def _backward():
        #   child_1.grad += <local gradient w/ respect to child_1> * out.grad
        #   child_2.grad += <local gradient w/ respect to child_2> * out.grad
        def _backward():
            self.grad += out.grad
            other.grad += out.grad
        out._backward = _backward

        return out
    
    def __sub__(self, other):
        out = Value(self.data - other.data, children=(self, other), op="-")

        def _backward():
            self.grad += out.grad
            other.grad -= out.grad
        out._backward = _backward

        return out

    def __mul__(self, other):
        out = Value(self.data * other.data, children=(self, other), op="*")

        def _backward():
            self.grad += other.data * out.grad
            other.grad += self.data * out.grad
        out._backward = _backward

        return out
    
    def __truediv__(self, other):
        out = Value(self.data / other.data, children=(self, other), op="/")
        
        def _backward():
            self.grad += (1 / other.data) * out.grad
            other.grad += (-self.data) * out.grad
        out._backward = _backward
        
        return out

    def __neg__(self):
        out = Value(-self.data, children=(self,), op="neg")

        def _backward():
            self.grad -= out.grad
        out._backward = _backward

        return out

    def tanh(self):
        t = (math.exp(2**self.data) - 1) / (math.exp(2**self.data) + 1)
        out = Value(t, children=(self,), op="tanh")

        def _backward():
            self.grad += (1 - t**2) * out.grad
        out._backward = _backward

        return out


def trace(root: Value):
    """Return graph of children from root.

    Find all the children, and all the edges connecting these values.

    Returns pair of sets. First set are all the values. The second set is a set
    of pairs of values which are the edges between the values.
    """
    seen, edges = set(), set()
    todo = [root]
    seen.add(root)
    while todo:
        curr = todo.pop()
        for child in curr.children:
            # Since duplicate nodes are never popped from todo
            # adding all the edges from curr never adds duplicate edges.
            # Since in neural nets values from from the child to the parents
            # add an edge from child to parent.
            edges.add((child, curr))
            if child not in seen:
                seen.add(child)
                # Duplicate notes are never added to todo because they are
                # always checked if they have been seen before.
                todo.append(child)
    return seen, edges


def draw_dot(root: Value):
    graph = graphviz.Digraph(format="svg", graph_attr={"rankdir": "LR"})
    nodes, edges = trace(root)
    for node in nodes:
        # For each Value create a node.
        uid = str(id(node))
        graph.node(uid, label="{%s | data: %.4f | grad: %.4f}" % (
            node.label, node.data, node.grad), shape="record")
        if node.op:
            # If the Value is also an op create another node for op, and then
            # connect from op to value node.
            graph.node(uid+node.op, label=node.op)
            graph.edge(uid+node.op, uid)
    for head, tail in edges:
        head_uid = str(id(head))
        tail_uid = str(id(tail)) + tail.op
        graph.edge(head_uid, tail_uid)
    return graph