import { pgTable, text, timestamp, pgEnum } from 'drizzle-orm/pg-core';

export const providerType = pgEnum("provider_type", ["google"]);

// Used by lucia
export const users = pgTable("user", {
    id: text("id").primaryKey(),
    createdAt: timestamp("created_at", {
        withTimezone: true,
        mode: "date"
    }).defaultNow()
});

// User by lucia
export const oauthAccounts = pgTable("oauth_account", {
    userId: text("user_id")
        .notNull()
        .references(() => users.id)
        .primaryKey(),
    provider: providerType("provider").notNull().primaryKey(),
    providerUserId: text("provider_user_id").notNull()
});

// Used by lucia
export const sessions = pgTable("session", {
    id: text("id").primaryKey(),
    userId: text("user_id")
        .notNull()
        .references(() => users.id),
    expiresAt: timestamp("expires_at", {
        withTimezone: true,
        mode: "date"
    }).notNull()
});
