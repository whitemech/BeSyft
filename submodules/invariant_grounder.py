#! /usr/bin/env python3

from itertools import product

if __name__ == "__main__":
    print("Grounding invariants...", end="")

    # 1. Open files of interest
    predicates_file = open("predicates_file.txt", "r")
    objects_file = open("objects_file.txt", "r")
    invariants_file = open("invariants_file.txt", "r")
    types_file = open("types_file.txt", "r")

    grounded_file = open("grounded_invs.txt", "w")

    # 2. Define types
    types_to_supertypes = dict()
    line_types = types_file.readline()
    while (line_types != ""):
        #print(line_types)
        line_list_type = line_types.split(" ")
        type_name = line_list_type[0].strip(":").strip("\n")
        try:
            type_basetype_str = line_list_type[1].strip("\n")
        except:
            type_basetype_str = type_name.strip("\n")
        if type_basetype_str in types_to_supertypes:
            types_to_supertypes[type_basetype_str].append(type_name)
        else:
            types_to_supertypes[type_basetype_str] = [type_name]
        line_types = types_file.readline()

    # for type in types_to_supertypes:
    #      print("type name: " + type + ". super types: " + str(types_to_supertypes[type]))

    types_file.close()

    # 3. Define predicates
    pred_to_types = dict()
    line = predicates_file.readline()
    while (line != ""):
        index = line.find("(")
        pred_name = line[0:index]
        pred_types_str = line[index+1:len(line)-2]
        pred_types_list = pred_types_str.split(" ")
        pred_types = []
        for i in range(1, len(pred_types_list), 2):
            pred_types.append(pred_types_list[i].strip(","))
        pred_to_types[pred_name] = pred_types
        line = predicates_file.readline()

    # for pred in pred_to_types:
    #      print("Predicate name: " + pred + ". Predicate types: " + str(pred_to_types[pred]))

    predicates_file.close()

    # 4. Assign objects to types
    supertypes_to_objs = dict()
    line = objects_file.readline()
    while (line != ""):
        line_list = line.split(" ")
        obj = line_list[0].strip(":")
        obj_type = line_list[1].strip("\n")
        if obj_type in supertypes_to_objs:
            supertypes_to_objs[obj_type].append(obj)
        else:
            supertypes_to_objs[obj_type] = [obj]
        line = objects_file.readline()

    # for obj_type in supertypes_to_objs:
    #     print("Supertype: " + obj_type + ". Objects: ", end="")
    #     for obj in supertypes_to_objs[obj_type]:
    #         print(obj + " ", end="")
    #     print()
    
    objects_file.close()

    types_to_objs = dict()
    for type in types_to_supertypes:
        for supertype in types_to_supertypes[type]:
            if type in types_to_objs:
                if supertype in supertypes_to_objs:
                    types_to_objs[type].extend(supertypes_to_objs[supertype])
            else:
                if supertype in supertypes_to_objs:
                    types_to_objs[type] = supertypes_to_objs[supertype]
            # if supertype in supertypes_to_objs:
            #     print(types_to_objs[type])
    types_to_objs.update(supertypes_to_objs)
    # for type in types_to_objs:
    #     print("Type: " + type + ". Objects: ", end="")
    #     for obj in types_to_objs[type]:
    #         print(obj + " ", end="")
    #     print()

    # 5. Ground invariants
    line = invariants_file.readline()
    while (line != ""):
        inv_string = ""
        line = line[1:len(line)-2]
        line_list = line.split("; ")
        #print("Invariant: ", line_list)
        for atom in line_list:
            #print("Atom: ", atom)
            atom = atom.strip("Atom")
            atom = atom.strip() # TODO. What happens with negative predicates?
            index = atom.find("(")
            pred_name = atom[0:index]
            atom_objs_str = atom[index+1:-1]
            atom_objs_list = atom_objs_str.split(", ")
            var_indexes = []
            for i in range(len(atom_objs_list)):
                if atom_objs_list[i].startswith("?"):
                    var_indexes.append(i)
            atom_types = pred_to_types[pred_name]
            product_args = []
            for i in range(0, len(atom_types)):
                if i in var_indexes:
                    type_objs = types_to_objs[(atom_types[i])]
                    product_args.append(type_objs)
                else:
                    product_args.append([atom_objs_list[i]])
            cartesian_product = product(*product_args)
            cartesian_product = list(cartesian_product)
            for i in range(0, len(cartesian_product)):
                if len(cartesian_product[i]) == 1:
                    inv_string +=  pred_name + str(cartesian_product[i]).replace(",", "").replace("'", "") + ";"
                else: 
                    inv_string +=  pred_name + str(cartesian_product[i]).replace("'", "") + ";"
        if inv_string.count(";") > 1: 
            grounded_file.write(inv_string.strip(";")+"\n")
        line = invariants_file.readline()

    invariants_file.close()
    grounded_file.close()

    print("Done!")