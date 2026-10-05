/*
 * COMSCI 1101 - FINAL GROUP PROJECT
 * Console-Based Mini POS System - BSIT MINI MART
 *
 * HOW THIS FILE IS ORGANIZED:
 * The program is divided into 9 PARTS.
 * Every part starts with its own INITIALIZATION (the variables and
 * arrays that the part needs), followed by its functions.
 *
 *   PART 1 - Main Menu            PART 6 - Checkout & Payment
 *   PART 2 - Product List         PART 7 - Update Stocks
 *   PART 3 - New Transaction      PART 8 - Print Receipt
 *   PART 4 - Stock Validation     PART 9 - Sales Summary
 *   PART 5 - Shopping Cart
 */

#include <stdio.h>

/* ---------- Function prototypes (a "list" of all functions) ---------- */
void  displayMenu();                 /* Part 1 */
void  displayProducts();             /* Part 2 */
void  newTransaction();              /* Part 3 */
int   getValidProductID();           /* Part 4 */
int   getValidQuantity(int index);   /* Part 4 */
float displayCart();                 /* Part 5 */
int   countCartItems();              /* Part 5 */
int   checkout(float subtotalAmount);/* Part 6 */
void  updateStocks();                /* Part 7 */
void  printReceipt();                /* Part 8 */
void  updateSalesSummary();          /* Part 9 */
void  salesSummary();                /* Part 9 */


/* ==================================================================
 *  PART 1 - MAIN MENU
 * ================================================================== */

/* ----- INITIALIZATION (Part 1) ----- */
int choice = 0;                      /* the menu choice of the user */

/* Shows the main menu. */
void displayMenu()
{
    printf("\n==========================================\n");
    printf("            MINI POS SYSTEM\n");
    printf("==========================================\n");
    printf("[1] View Products\n");
    printf("[2] New Transaction\n");
    printf("[3] Sales Summary\n");
    printf("[4] Exit\n");
    printf("==========================================\n");
    printf("Enter your choice: ");
}

/* main() repeats the menu until the user chooses Exit. */
int main()
{
    do
    {
        displayMenu();
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                displayProducts();
                break;
            case 2:
                newTransaction();
                break;
            case 3:
                salesSummary();
                break;
            case 4:
                printf("\nThank you! Goodbye!\n");
                break;
            default:
                printf("\nERROR: Invalid choice! Enter 1 to 4.\n");
        }
    } while (choice != 4);

    return 0;
}


/* ==================================================================
 *  PART 2 - PRODUCT LIST
 * ================================================================== */

/* ----- INITIALIZATION (Part 2) ----- */
#define MAX_PRODUCTS 5               /* number of products */

/* Index 0 = Product ID 1, index 1 = Product ID 2, and so on. */
char  productNames[MAX_PRODUCTS][20] = {"Coke", "Sprite", "Piattos", "Water", "Bread"};
float prices[MAX_PRODUCTS]           = {25.00, 25.00, 30.00, 15.00, 40.00};
int   stocks[MAX_PRODUCTS]           = {20, 15, 10, 25, 10};

/* Shows all products using a for loop. */
void displayProducts()
{
    int i;

    printf("\n=============================================\n");
    printf("PRODUCT LIST\n");
    printf("=============================================\n");
    printf("%-4s %-14s %8s %6s\n", "ID", "PRODUCT", "PRICE", "STOCK");
    printf("---------------------------------------------\n");
    for (i = 0; i < MAX_PRODUCTS; i++)
    {
        printf("%-4d %-14s %8.2f %6d\n", i + 1, productNames[i], prices[i], stocks[i]);
    }
    printf("=============================================\n");
}


/* ==================================================================
 *  PART 3 - NEW TRANSACTION
 * ================================================================== */

/* ----- INITIALIZATION (Part 3) ----- */
/* cartQty[0] = how many Coke the customer is buying,
   cartQty[1] = how many Sprite, and so on (same order as the products). */
int cartQty[MAX_PRODUCTS] = {0, 0, 0, 0, 0};

/* Lets the cashier add many products, then goes to checkout. */
void newTransaction()
{
    int   i;
    int   productID, index, quantity;
    int   addMore;
    float subtotalAmount;

    /* Start a new cart: set every quantity to 0 */
    for (i = 0; i < MAX_PRODUCTS; i++)
    {
        cartQty[i] = 0;
    }

    do
    {
        displayProducts();

        productID = getValidProductID();          /* Part 4 */
        index = productID - 1;                    /* ID 1 is index 0 */
        quantity = getValidQuantity(index);       /* Part 4 */

        if (quantity > 0)
        {
            cartQty[index] = cartQty[index] + quantity;   /* add to cart */

            printf("\nProduct: %s\n", productNames[index]);
            printf("Price: P%.2f\n", prices[index]);
            printf("Quantity: %d\n", quantity);
            printf("Item Total: P%.2f\n", prices[index] * quantity);
            printf("Item successfully added!\n");
        }

        printf("\nAdd another item?\n");
        printf("[1] Yes\n");
        printf("[2] Proceed to Checkout\n");
        printf("Enter choice: ");
        scanf("%d", &addMore);

        while (addMore != 1 && addMore != 2)
        {
            printf("ERROR: Please enter 1 or 2: ");
            scanf("%d", &addMore);
        }
    } while (addMore == 1);

    /* Nothing was bought */
    if (countCartItems() == 0)
    {
        printf("\nCart is empty. Transaction cancelled.\n");
        return;
    }

    subtotalAmount = displayCart();               /* Part 5 */

    if (checkout(subtotalAmount) == 1)            /* Part 6 */
    {
        printReceipt();                           /* Part 8 */
        updateStocks();                           /* Part 7 */
        updateSalesSummary();                     /* Part 9 */
    }
}


/* ==================================================================
 *  PART 4 - STOCK VALIDATION
 *  Rejects: wrong product ID, zero quantity, negative quantity,
 *           and quantity greater than the available stock.
 * ================================================================== */

/* ----- INITIALIZATION (Part 4) ----- */
int enteredID      = 0;              /* product ID typed by the cashier */
int enteredQty     = 0;              /* quantity typed by the cashier */
int availableStock = 0;              /* stock that can still be bought */

/* Asks for a Product ID until it is from 1 to 5. */
int getValidProductID()
{
    printf("Enter Product ID: ");
    scanf("%d", &enteredID);

    while (enteredID < 1 || enteredID > MAX_PRODUCTS)
    {
        printf("ERROR: Invalid Product ID! Enter 1 to %d: ", MAX_PRODUCTS);
        scanf("%d", &enteredID);
    }
    return enteredID;
}

/* Asks for a quantity until it is valid.
   Returns 0 if the product is already out of stock. */
int getValidQuantity(int index)
{
    /* stock in store minus what is already in the cart */
    availableStock = stocks[index] - cartQty[index];

    if (availableStock <= 0)
    {
        printf("ERROR: %s is out of stock!\n", productNames[index]);
        return 0;
    }

    printf("Enter Quantity: ");
    scanf("%d", &enteredQty);

    while (enteredQty <= 0 || enteredQty > availableStock)
    {
        if (enteredQty <= 0)
        {
            printf("ERROR: Quantity must be greater than zero!\n");
        }
        else
        {
            printf("Available Stock: %d\n", availableStock);
            printf("ERROR: Insufficient stock!\n");
        }
        printf("Please enter a valid quantity: ");
        scanf("%d", &enteredQty);
    }
    return enteredQty;
}


/* ==================================================================
 *  PART 5 - SHOPPING CART
 * ================================================================== */

/* ----- INITIALIZATION (Part 5) ----- */
/* The cart uses the cartQty[] array from Part 3.
   The variables below are inside each function. */

/* Shows the cart and returns the subtotal. */
float displayCart()
{
    int   i;
    float itemTotal;
    float subtotalAmount = 0;

    printf("\n=============================================\n");
    printf("YOUR CART\n");
    printf("=============================================\n");
    printf("%-14s %4s %8s %9s\n", "PRODUCT", "QTY", "PRICE", "TOTAL");
    printf("---------------------------------------------\n");
    for (i = 0; i < MAX_PRODUCTS; i++)
    {
        if (cartQty[i] > 0)                       /* show only bought items */
        {
            itemTotal = prices[i] * cartQty[i];
            subtotalAmount = subtotalAmount + itemTotal;
            printf("%-14s %4d %8.2f %9.2f\n", productNames[i], cartQty[i], prices[i], itemTotal);
        }
    }
    printf("---------------------------------------------\n");
    printf("SUBTOTAL: P%.2f\n", subtotalAmount);
    printf("=============================================\n");

    return subtotalAmount;
}

/* Counts all items in the cart. */
int countCartItems()
{
    int i;
    int totalItems = 0;

    for (i = 0; i < MAX_PRODUCTS; i++)
    {
        totalItems = totalItems + cartQty[i];
    }
    return totalItems;
}


/* ==================================================================
 *  PART 6 - CHECKOUT & PAYMENT
 * ================================================================== */

/* ----- INITIALIZATION (Part 6) ----- */
/* These are global so the receipt (Part 8) can also use them. */
float subtotal     = 0;
float discountRate = 0;
float discount     = 0;
float total        = 0;
float cash         = 0;
float change       = 0;

/* Asks the customer type, computes the total, asks for cash.
   Returns 1 when the payment is successful. */
int checkout(float subtotalAmount)
{
    int customerType;

    subtotal = subtotalAmount;

    printf("\nCustomer Type:\n");
    printf("[1] Regular Customer      0%% Discount\n");
    printf("[2] Discounted Customer  10%% Discount\n");
    printf("Enter choice: ");
    scanf("%d", &customerType);

    while (customerType != 1 && customerType != 2)
    {
        printf("ERROR: Please enter 1 or 2: ");
        scanf("%d", &customerType);
    }

    switch (customerType)
    {
        case 1:
            discountRate = 0.00;
            break;
        case 2:
            discountRate = 0.10;
            break;
    }

    discount = subtotal * discountRate;           /* Discount = Subtotal x Discount Rate */
    total = subtotal - discount;                  /* Total = Subtotal - Discount */

    printf("\nSubtotal: P%.2f\n", subtotal);
    printf("Discount: P%.2f\n", discount);
    printf("-----------------------\n");
    printf("TOTAL: P%.2f\n", total);

    printf("Enter Cash: P");
    scanf("%f", &cash);

    /* Keep asking until the cash is enough */
    while (cash < total)
    {
        printf("Insufficient payment!\n");
        printf("Enter Cash: P");
        scanf("%f", &cash);
    }

    change = cash - total;                        /* Change = Cash - Total */

    printf("\nPayment Successful!\n");
    printf("Total: P%.2f\n", total);
    printf("Cash: P%.2f\n", cash);
    printf("Change: P%.2f\n", change);

    return 1;
}


/* ==================================================================
 *  PART 7 - UPDATE STOCKS
 *  Runs only after a successful payment.
 * ================================================================== */

/* ----- INITIALIZATION (Part 7) ----- */
/* Uses stocks[] (Part 2) and cartQty[] (Part 3).
   The variables below are inside the function. */

/* Subtracts the bought quantities from the stocks. */
void updateStocks()
{
    int i;
    int oldStock;

    printf("\n=============================================\n");
    printf("STOCK UPDATE\n");
    printf("=============================================\n");
    for (i = 0; i < MAX_PRODUCTS; i++)
    {
        if (cartQty[i] > 0)
        {
            oldStock = stocks[i];                         /* stock before */
            stocks[i] = stocks[i] - cartQty[i];           /* DEDUCT the bought quantity */
            printf("%s Stock: %d - %d = %d\n", productNames[i], oldStock, cartQty[i], stocks[i]);
        }
    }
    printf("=============================================\n");
}


/* ==================================================================
 *  PART 8 - PRINT RECEIPT
 * ================================================================== */

/* ----- INITIALIZATION (Part 8) ----- */
/* Uses the values from Part 6 (subtotal, discount, total, cash, change)
   and the cart from Part 3. */

/* Prints the receipt. */
void printReceipt()
{
    int i;

    printf("\n=============================================\n");
    printf("              BSIT MINI MART\n");
    printf("=============================================\n");
    printf("                  RECEIPT\n");
    printf("---------------------------------------------\n");
    printf("%-14s %4s %8s %9s\n", "PRODUCT", "QTY", "PRICE", "TOTAL");
    printf("---------------------------------------------\n");
    for (i = 0; i < MAX_PRODUCTS; i++)
    {
        if (cartQty[i] > 0)
        {
            printf("%-14s %4d %8.2f %9.2f\n", productNames[i], cartQty[i], prices[i], prices[i] * cartQty[i]);
        }
    }
    printf("---------------------------------------------\n");
    printf("Subtotal: P%.2f\n", subtotal);
    printf("Discount: P%.2f\n", discount);
    printf("TOTAL: P%.2f\n", total);
    printf("Cash: P%.2f\n", cash);
    printf("Change: P%.2f\n", change);
    printf("---------------------------------------------\n");
    printf("Total Items Purchased: %d\n", countCartItems());
    printf("---------------------------------------------\n");
    printf("                THANK YOU!\n");
    printf("=============================================\n");
}


/* ==================================================================
 *  PART 9 - SALES SUMMARY
 * ================================================================== */

/* ----- INITIALIZATION (Part 9) ----- */
int   numTransactions = 0;           /* how many successful sales */
int   totalItemsSold  = 0;           /* how many items were sold */
float totalSales      = 0;           /* total money earned */

/* Called after every successful payment. */
void updateSalesSummary()
{
    numTransactions = numTransactions + 1;
    totalItemsSold = totalItemsSold + countCartItems();
    totalSales = totalSales + total;
}

/* Shows the sales summary. */
void salesSummary()
{
    printf("\n=============================================\n");
    printf("               SALES SUMMARY\n");
    printf("=============================================\n");
    printf("Number of Transactions: %d\n", numTransactions);
    printf("Total Items Sold: %d\n", totalItemsSold);
    printf("Total Sales: P%.2f\n", totalSales);
    printf("=============================================\n");
}