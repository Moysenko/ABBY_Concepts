from collections import defaultdict


def _LINQ_object_from_iterator(iterator):
    def wrapper(*args, **kwargs):
        return LINQ(iterator(*args, **kwargs)) 
    return wrapper


class LINQ:
    def __init__(self, iterator):
        self.iterator = iterator
        
    def __iter__(self):
        for x in self.iterator:
            yield x
    
    @_LINQ_object_from_iterator
    def select(self, function):
        for x in self:
            yield function(x)

    @_LINQ_object_from_iterator
    def flatten(self):
        for subsequence in self:
            for x in subsequence:
                yield x

    @_LINQ_object_from_iterator
    def where(self, predicate):
        for x in self:
            if predicate(x):
                yield x

    @_LINQ_object_from_iterator
    def take(self, num_elements):
        for i, x in enumerate(self):
            if i >= num_elements:
                break
            yield x

    @_LINQ_object_from_iterator
    def group_by(self, key):
        # Не может быть ленивой
        mapping = defaultdict(list)
        for x in self:
            mapping[key(x)].append(x)
        return mapping.items()

    @_LINQ_object_from_iterator
    def order_by(self, key):
        # Не может быть ленивой
        for x in sorted(self, key=key):
            yield x

    def to_list(self):
        return list(self)

    def to_stdout(self):
        print('\n'.join(list(map(str, self))))
