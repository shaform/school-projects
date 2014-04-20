"""vector space model"""

def sim(v1, v2):
    """Compute similarity between vector v1 & v2,
    assuming that both v1, v2 are normalized."""

    if len(v1) > len(v2):
        v1, v2 = v2, v1

    total = 0.0
    for key in v1.keys():
        if key in v2:
            total += v1[key] * v2[key]
    return total
