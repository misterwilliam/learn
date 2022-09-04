import typing

class Value:

    def __init__(self, data: typing.Union[float, int]):
        if isinstance(data, float):
            self.data = data
            return
        if isinstance(data, int):
            self.data = float(data)
            return
        
        raise TypeError("Values can only be floats or ints.")

    def __repr__(self):
        return f"Value(data={self.data})"

    def __add__(self, other):
        return Value(self.data + other.data)

    def __mul__(self, other):
        return Value(self.data * other.data)

    def __sub__(self, other):
        return Value(self.data - other.data)

    def __neg__(self):
        return Value(-self.data)