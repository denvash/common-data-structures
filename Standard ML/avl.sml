datatype 'a AVLTree = Nil | Br of ((int*('a))*('a AVLTree)*('a AVLTree));

datatype Balance = RR | LR | LL | RL;

exception NotFound;

(* val inorder = fn: ('a AVLTree)->`a list *)
(* Time: O(n) *)
fun inorder Nil = []
		| inorder (Br ((_,v), l_tree, r_tree)) = inorder r_tree @ [v] @ inorder l_tree;

(* val size = fn: 'a AVLTree->int *)
(* Time: O(n) *)
fun size Nil = 0
		| size avl_tree = List.length (inorder avl_tree);

(* val get = fn: ('a AVLTree*int)->`a *)
(* Time: O(log n) *)
fun get (Nil,_) = raise NotFound
	| get ((Br ((k,v),lt,rt)),k_) =
		case Int.compare(k,k_) of
        LESS => get(lt,k_)
      | EQUAL => v
      | GREATER => get(rt,k_)
;

local
  (* val height = fn : 'a AVLTree -> int *)
  (* Time O(n) *)
	fun height Nil = 0
		| height (Br(_,l_tree,r_tree)) = 1 + Int.max(height l_tree, height r_tree)
  ;

	(* val rotate_left = fn : 'a AVLTree -> 'a AVLTree *)
  fun rotate_left Nil = Nil
    | rotate_left (Br(_,Nil,_)) = Nil
    | rotate_left (Br(v , Br(vl,llt,lrt) ,rt)) = Br(vl, llt ,Br(v,lrt,rt))
  ;

	(* val rotate_right = fn : 'a AVLTree -> 'a AVLTree *)
  fun rotate_right Nil = Nil
    | rotate_right (Br(_,_,Nil)) = Nil
    | rotate_right (Br(v,lt,Br(vr,rlt,rrt))) = Br(vr, Br(v,lt,rlt) ,rrt)
  ;

  (* val min = fn : 'a AVLTree -> int * 'a *)
  (* Time O(log n) *)
  fun min Nil = raise NotFound
    | min ( Br(node,Nil, _)) = node
    | min ( Br(_, l_tree, _)) = min l_tree
  ;

  (* val max = fn : 'a AVLTree -> int * 'a *)
  (* Time O(log n) *)
  fun max Nil = raise NotFound
    | max ( Br(node,_, Nil) ) = node
    | max ( Br(_, _, r_tree) )  = max r_tree
  ;

in
	(* val rotate = fn : 'a AVLTree -> Balance -> 'a AVLTree *)
	fun rotate Nil _ = Nil
		| rotate tree MODE =

			case MODE of
				  RR => rotate_right tree
				| RL => rotate_left ( rotate_right tree )
				| LL => rotate_left tree
				| LR => rotate_right ( rotate_left tree )
	;

	(* val insert = fn: ('a AVLTree*(int*'a))->'a AVLTree *)
	fun insert ( Nil,singleton ) = Br (singleton,Nil,Nil)
		| insert ( Br( node as (k,v) , l_tree ,r_tree ) , t_node as (t_k,t_v) ) =

			case Int.compare(k,t_k) of
					LESS =>
						let
							val n_l_tree as Br ((n_l_k,n_l_v),_,_) = insert(l_tree,t_node)
							val n_tree = Br (node,n_l_tree,r_tree)
						in
							if height n_l_tree - height r_tree = 2
								then
									if t_k < n_l_k
										then rotate n_tree LL
									else rotate n_tree LR
							else n_tree
						end

					| EQUAL => Br ((k,t_v),l_tree,r_tree)

					| GREATER  =>
						let
							val n_r_tree as Br((n_r_k,n_r_v),_,_) = insert(r_tree,t_node)
							val n_tree = Br(node, l_tree, n_r_tree)
						in
							if height n_r_tree - height l_tree = 2
								then
									if t_k > n_r_k
										then rotate n_tree RR
									else rotate n_tree RL
							else n_tree
						end
	;

  (* val remove = fn: ('a AVLTree*int)->'a AVLTree *)
  fun remove (Nil, _) = Nil
    | remove (Br( node as (k,v), l_tree, r_tree), t_k) =

      let
        fun balance_right_left Nil = Nil
          | balance_right_left (Br(_,_,Nil)) = Nil
          | balance_right_left (t as Br(_ ,lt, Br(_ ,rlt, rrt))) =
            if height rrt >= height rlt
             then rotate t RR
            else rotate t LR
        ;

        fun balance_left_right Nil = Nil
          | balance_left_right (Br(_,Nil,_)) = Nil
          | balance_left_right (t as Br(_, Br(_,llt, lrt), rt)) =
                  if height llt >= height lrt
                   then rotate t LL
                  else rotate t RL
        ;
      in

        case Int.compare(k,t_k) of
            GREATER =>
              let
                val n_r_tree = remove(r_tree,t_k)
                val n_tree = Br( node, l_tree, n_r_tree)
              in
                if height l_tree = height n_r_tree + 2
                  then balance_left_right n_tree
                else n_tree
              end

          | LESS =>
            let
              val n_l_tree = remove(l_tree,t_k)
              val n_tree = Br (node, n_l_tree, r_tree)
            in
              if height r_tree = height n_l_tree + 2
                then balance_right_left n_tree
              else n_tree
            end

          | EQUAL =>
            case (l_tree, r_tree) of
                (Nil , _) => r_tree
              | (_,  Nil) => l_tree
              |  _        => if height l_tree <= height r_tree
                            then
                              let
                                val m as (m_k,_) = min r_tree
                              in
                                Br(m, l_tree ,remove(r_tree, m_k))
                              end

                            else
                              let
                                val m as (m_k,_) = max l_tree
                              in
                                Br(m, remove(l_tree, m_k), r_tree)
                              end
      end
  ;
end